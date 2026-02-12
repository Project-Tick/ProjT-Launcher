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
 */
#include "java/download/RuntimeManifestTask.hpp"

#include <QFile>
#include <QJsonDocument>

#include "Application.h"
#include "FileSystem.h"
#include "Json.h"
#include "net/ChecksumValidator.h"
#include "net/NetJob.h"

namespace
{
	struct FileEntry
	{
		QString path;
		QString url;
		QByteArray hash;
		bool isExec = false;
	};
} // namespace

namespace projt::java
{
	RuntimeManifestTask::RuntimeManifestTask(QUrl url, QString finalPath, QString checksumType, QString checksumHash)
		: m_url(url),
		  m_final_path(std::move(finalPath)),
		  m_checksum_type(std::move(checksumType)),
		  m_checksum_hash(std::move(checksumHash))
	{}

	void RuntimeManifestTask::executeTask()
	{
		setStatus(tr("Downloading Java"));
		auto download = makeShared<NetJob>(QString("JRE::DownloadJava"), APPLICATION->network());
		auto files	  = std::make_shared<QByteArray>();

		auto action = Net::Download::makeByteArray(m_url, files);
		if (!m_checksum_hash.isEmpty() && !m_checksum_type.isEmpty())
		{
			auto hashType = QCryptographicHash::Algorithm::Sha1;
			if (m_checksum_type == "sha256")
			{
				hashType = QCryptographicHash::Algorithm::Sha256;
			}
			action->addValidator(new Net::ChecksumValidator(hashType, QByteArray::fromHex(m_checksum_hash.toUtf8())));
		}
		download->addNetAction(action);

		connect(download.get(), &Task::failed, this, &RuntimeManifestTask::emitFailed);
		connect(download.get(), &Task::progress, this, &RuntimeManifestTask::setProgress);
		connect(download.get(), &Task::stepProgress, this, &RuntimeManifestTask::propagateStepProgress);
		connect(download.get(), &Task::status, this, &RuntimeManifestTask::setStatus);
		connect(download.get(), &Task::details, this, &RuntimeManifestTask::setDetails);

		connect(download.get(),
				&Task::succeeded,
				[files, this]
				{
					QJsonParseError parse_error{};
					QJsonDocument doc = QJsonDocument::fromJson(*files, &parse_error);
					if (parse_error.error != QJsonParseError::NoError)
					{
						qWarning() << "Error while parsing JSON response at " << parse_error.offset
								   << ". Reason: " << parse_error.errorString();
						qWarning() << *files;
						emitFailed(parse_error.errorString());
						return;
					}
					downloadRuntime(doc);
				});
		m_task = download;
		m_task->start();
	}

	void RuntimeManifestTask::downloadRuntime(const QJsonDocument& doc)
	{
		FS::ensureFolderPathExists(m_final_path);
		std::vector<FileEntry> toDownload;
		auto list = Json::ensureObject(Json::ensureObject(doc.object()), "files");
		for (const auto& pathKey : list.keys())
		{
			auto filePath			= FS::PathCombine(m_final_path, pathKey);
			const QJsonObject& meta = Json::ensureObject(list, pathKey);
			auto type				= Json::ensureString(meta, "type");
			if (type == "directory")
			{
				FS::ensureFolderPathExists(filePath);
			}
			else if (type == "link")
			{
				auto target = Json::ensureString(meta, "target");
				if (!target.isEmpty())
				{
					QFile::link(target, filePath);
				}
			}
			else if (type == "file")
			{
				auto downloads = Json::ensureObject(meta, "downloads");
				auto isExec	   = Json::ensureBoolean(meta, "executable", false);
				QString url;
				QByteArray hash;

				if (downloads.contains("raw"))
				{
					auto raw = Json::ensureObject(downloads, "raw");
					url		 = Json::ensureString(raw, "url");
					hash	 = QByteArray::fromHex(Json::ensureString(raw, "sha1").toLatin1());
				}
				else
				{
					qWarning() << "No raw download available for file:" << pathKey;
					qWarning() << "Skipping file without raw download - decompression not yet supported";
					continue;
				}

				if (!url.isEmpty() && QUrl(url).isValid())
				{
					toDownload.push_back({ filePath, url, hash, isExec });
				}
			}
		}
		auto elementDownload = makeShared<NetJob>("JRE::FileDownload", APPLICATION->network());
		for (const auto& file : toDownload)
		{
			auto dl = Net::Download::makeFile(file.url, file.path);
			if (!file.hash.isEmpty())
			{
				dl->addValidator(new Net::ChecksumValidator(QCryptographicHash::Sha1, file.hash));
			}
			if (file.isExec)
			{
				connect(dl.get(),
						&Net::Download::succeeded,
						[file]
						{
							QFile(file.path).setPermissions(QFile(file.path).permissions()
															| QFileDevice::Permissions(0x1111));
						});
			}
			elementDownload->addNetAction(dl);
		}

		connect(elementDownload.get(), &Task::failed, this, &RuntimeManifestTask::emitFailed);
		connect(elementDownload.get(), &Task::progress, this, &RuntimeManifestTask::setProgress);
		connect(elementDownload.get(), &Task::stepProgress, this, &RuntimeManifestTask::propagateStepProgress);
		connect(elementDownload.get(), &Task::status, this, &RuntimeManifestTask::setStatus);
		connect(elementDownload.get(), &Task::details, this, &RuntimeManifestTask::setDetails);

		connect(elementDownload.get(), &Task::succeeded, this, &RuntimeManifestTask::emitSucceeded);
		m_task = elementDownload;
		m_task->start();
	}

	bool RuntimeManifestTask::abort()
	{
		auto aborted = canAbort();
		if (m_task)
			aborted = m_task->abort();
		emitAborted();
		return aborted;
	}
} // namespace projt::java
