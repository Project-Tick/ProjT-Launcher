// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2026 Project Tick
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * ========================================================================
 */

#include "ModrinthCollectionImportTask.h"

#include "Application.h"
#include "Json.h"
#include "minecraft/MinecraftInstance.h"
#include "minecraft/PackProfile.h"
#include "modplatform/modrinth/ModrinthAPI.h"
#include "net/ApiDownload.h"
#include "net/NetJob.h"

#include <QJsonDocument>
#include <QRegularExpression>
#include <QSet>
#include <QUrl>

#include <utility>

namespace
{
	constexpr int kProjectBatchSize = 50;
}

ModrinthCollectionImportTask::ModrinthCollectionImportTask(QString collection_reference, MinecraftInstance* instance)
	: Task(false),
	  m_collection_reference(std::move(collection_reference).trimmed()),
	  m_instance(instance)
{
	if (m_instance)
	{
		auto pack_profile = m_instance->getPackProfile();
		if (auto minecraft = pack_profile->getComponent("net.minecraft"); minecraft)
			m_mc_versions.push_back(minecraft->getVersion());
		m_loaders = pack_profile->getSupportedModLoaders().value_or(ModPlatform::ModLoaderTypes{});
	}
}

bool ModrinthCollectionImportTask::abort()
{
	if (!canAbort())
		return false;

	if (m_current_task)
		return m_current_task->abort();

	emitAborted();
	return true;
}

void ModrinthCollectionImportTask::executeTask()
{
	setAbortable(true);

	if (!m_instance)
	{
		emitFailed(tr("Missing instance context for Modrinth collection import."));
		return;
	}

	m_collection_reference = normalizeCollectionReference(m_collection_reference);
	if (m_collection_reference.isEmpty())
	{
		emitFailed(tr("Enter a valid Modrinth collection URL or collection ID."));
		return;
	}

	fetchCollectionPage();
}

QString ModrinthCollectionImportTask::normalizeCollectionReference(QString reference) const
{
	reference = reference.trimmed();
	if (reference.isEmpty())
		return {};

	static const QRegularExpression collectionPathPattern(R"(^(?:/?collection/)?([^/?#]+))");

	if (reference.startsWith("http://") || reference.startsWith("https://"))
	{
		QUrl url(reference);
		if (!url.isValid())
			return {};

		const auto host = url.host();
		if (host != "modrinth.com" && host != "www.modrinth.com")
			return {};

		auto match = collectionPathPattern.match(url.path().mid(1));
		return match.hasMatch() ? match.captured(1) : QString{};
	}

	auto match = collectionPathPattern.match(reference);
	return match.hasMatch() ? match.captured(1) : reference;
}

QString ModrinthCollectionImportTask::collectionUrl() const
{
	return QString("https://modrinth.com/collection/%1")
		.arg(QString::fromUtf8(QUrl::toPercentEncoding(m_collection_reference)));
}

bool ModrinthCollectionImportTask::extractCollectionMetadata(const QByteArray& html)
{
	QString text = QString::fromUtf8(html);

	static const QRegularExpression titlePattern(R"(<title>([^<]+)</title>)");
	if (auto match = titlePattern.match(text); match.hasMatch())
	{
		m_collection_name	 = match.captured(1).trimmed();
		const QString suffix = QStringLiteral(" - Modrinth");
		if (m_collection_name.endsWith(suffix))
			m_collection_name.chop(suffix.size());
	}

	QStringList best_ids;
	static const QRegularExpression projectsPattern(R"(projects\?ids=%5B([^"]+)%5D)");
	auto match_it = projectsPattern.globalMatch(text);
	while (match_it.hasNext())
	{
		auto match = match_it.next();
		QStringList ids;
		auto decoded = QUrl::fromPercentEncoding(match.captured(1).toUtf8());

		static const QRegularExpression idPattern(QStringLiteral("\"([A-Za-z0-9]+)\""));
		auto id_it = idPattern.globalMatch(decoded);
		QSet<QString> seen;
		while (id_it.hasNext())
		{
			auto id_match = id_it.next();
			auto id		  = id_match.captured(1);
			if (!seen.contains(id))
			{
				seen.insert(id);
				ids.append(id);
			}
		}

		if (ids.size() > best_ids.size())
			best_ids = ids;
	}

	m_project_ids = best_ids;
	return !m_project_ids.isEmpty();
}

void ModrinthCollectionImportTask::fetchCollectionPage()
{
	setStatus(tr("Loading Modrinth collection page..."));
	setProgress(0, 1);

	auto response = std::make_shared<QByteArray>();
	auto job	  = makeShared<NetJob>(tr("Modrinth collection page"), APPLICATION->network());
	job->addNetAction(Net::ApiDownload::makeByteArray(QUrl(collectionUrl()), response));

	connect(job.get(),
			&NetJob::succeeded,
			this,
			[this, response]
			{
				if (!extractCollectionMetadata(*response))
				{
					emitFailed(tr("Could not extract any projects from this Modrinth collection page."));
					return;
				}

				const int project_batches = (m_project_ids.size() + kProjectBatchSize - 1) / kProjectBatchSize;
				setProgress(1, 1 + project_batches + m_project_ids.size());
				QMetaObject::invokeMethod(this, &ModrinthCollectionImportTask::fetchProjectBatch, Qt::QueuedConnection);
			});
	connect(job.get(), &NetJob::failed, this, &ModrinthCollectionImportTask::emitFailed);
	connect(job.get(), &NetJob::aborted, this, &ModrinthCollectionImportTask::emitAborted);
	connect(job.get(), &NetJob::stepProgress, this, &ModrinthCollectionImportTask::propagateStepProgress);

	m_current_task = job;
	job->start();
}

void ModrinthCollectionImportTask::fetchProjectBatch()
{
	if (m_project_batch_offset >= m_project_ids.size())
	{
		QHash<QString, ModPlatform::IndexedPack::Ptr> pack_lookup;
		for (auto& pack : std::as_const(m_packs))
			pack_lookup.insert(pack->addonId.toString(), pack);

		QList<ModPlatform::IndexedPack::Ptr> ordered_packs;
		for (auto& id : std::as_const(m_project_ids))
		{
			if (pack_lookup.contains(id))
				ordered_packs.append(pack_lookup.value(id));
		}
		m_packs = ordered_packs;

		if (m_packs.isEmpty())
		{
			emitFailed(
				tr("This collection does not contain any projects that could be resolved through the Modrinth API."));
			return;
		}

		QMetaObject::invokeMethod(this, &ModrinthCollectionImportTask::fetchNextVersion, Qt::QueuedConnection);
		return;
	}

	setStatus(tr("Loading collection projects..."));

	auto response	 = std::make_shared<QByteArray>();
	const auto batch = m_project_ids.mid(m_project_batch_offset, kProjectBatchSize);
	auto job		 = m_api.getProjects(batch, response);
	if (!job)
	{
		emitFailed(tr("Failed to request Modrinth project metadata for this collection."));
		return;
	}

	connect(job.get(),
			&Task::succeeded,
			this,
			[this, response, batch]
			{
				QJsonParseError parse_error{};
				QJsonDocument doc = QJsonDocument::fromJson(*response, &parse_error);
				if (parse_error.error != QJsonParseError::NoError)
				{
					emitFailed(tr("Failed to parse Modrinth project metadata for this collection."));
					return;
				}

				if (!doc.isArray())
				{
					emitFailed(tr("Modrinth returned an unexpected project list for this collection."));
					return;
				}

				QSet<QString> seen_in_batch;
				for (auto entry : doc.array())
				{
					try
					{
						auto obj  = Json::requireObject(entry);
						auto pack = std::make_shared<ModPlatform::IndexedPack>();
						m_api.loadIndexedPack(*pack, obj);
						m_api.loadExtraPackInfo(*pack, obj);
						if (!seen_in_batch.contains(pack->addonId.toString()))
						{
							m_packs.append(pack);
							seen_in_batch.insert(pack->addonId.toString());
						}
					}
					catch (const JSONValidationError&)
					{
						continue;
					}
				}

				m_project_batch_offset += batch.size();
				setProgress(getProgress() + 1, getTotalProgress());
				QMetaObject::invokeMethod(this, &ModrinthCollectionImportTask::fetchProjectBatch, Qt::QueuedConnection);
			});
	connect(job.get(), &Task::failed, this, &ModrinthCollectionImportTask::emitFailed);
	connect(job.get(), &Task::aborted, this, &ModrinthCollectionImportTask::emitAborted);
	connect(job.get(), &Task::stepProgress, this, &ModrinthCollectionImportTask::propagateStepProgress);

	m_current_task = job;
	job->start();
}

void ModrinthCollectionImportTask::fetchNextVersion()
{
	if (m_version_index >= m_packs.size())
	{
		finishImport();
		return;
	}

	auto pack = m_packs.at(m_version_index);
	setStatus(tr("Resolving a compatible version for %1...").arg(pack->name));

	ResourceAPI::Callback<QVector<ModPlatform::IndexedVersion>> callbacks;
	callbacks.on_succeed = [this, pack](auto& versions)
	{
		if (!versions.isEmpty())
			m_imported_resources.append({ pack, versions.first() });
		else
			m_skipped_resources.append(pack->name);

		++m_version_index;
		setProgress(getProgress() + 1, getTotalProgress());
		QMetaObject::invokeMethod(this, &ModrinthCollectionImportTask::fetchNextVersion, Qt::QueuedConnection);
	};
	callbacks.on_fail = [this, pack](QString const&, int)
	{
		m_skipped_resources.append(pack->name);
		++m_version_index;
		setProgress(getProgress() + 1, getTotalProgress());
		QMetaObject::invokeMethod(this, &ModrinthCollectionImportTask::fetchNextVersion, Qt::QueuedConnection);
	};
	callbacks.on_abort = [this] { emitAborted(); };

	ResourceAPI::VersionSearchArgs args{ pack, m_mc_versions, m_loaders, ModPlatform::ResourceType::Mod };
	auto job = m_api.getProjectVersions(std::move(args), std::move(callbacks));
	if (!job)
	{
		m_skipped_resources.append(pack->name);
		++m_version_index;
		setProgress(getProgress() + 1, getTotalProgress());
		QMetaObject::invokeMethod(this, &ModrinthCollectionImportTask::fetchNextVersion, Qt::QueuedConnection);
		return;
	}

	connect(job.get(), &Task::stepProgress, this, &ModrinthCollectionImportTask::propagateStepProgress);

	m_current_task = job;
	job->start();
}

void ModrinthCollectionImportTask::finishImport()
{
	if (m_imported_resources.isEmpty())
	{
		emitFailed(tr("No compatible mods were found in this collection for the current instance."));
		return;
	}

	if (!m_skipped_resources.isEmpty())
	{
		logWarning(tr("Skipped %1 project(s) without a compatible version: %2")
					   .arg(m_skipped_resources.size())
					   .arg(m_skipped_resources.join(", ")));
	}

	emitSucceeded();
}
