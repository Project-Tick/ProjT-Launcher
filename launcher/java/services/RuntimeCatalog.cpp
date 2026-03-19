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

#include "java/services/RuntimeCatalog.hpp"

#include <QtNetwork>
#include <QtXml>

#include <QDebug>
#include <algorithm>

#include "Application.h"
#include "java/services/RuntimeScanner.hpp"
#include "tasks/ConcurrentTask.h"

namespace projt::java
{
	RuntimeCatalog::RuntimeCatalog(QObject* parent, Scope scope) : BaseVersionList(parent), m_scope(scope)
	{}

	Task::Ptr RuntimeCatalog::getLoadTask()
	{
		load();
		return currentTask();
	}

	Task::Ptr RuntimeCatalog::currentTask() const
	{
		if (m_state == State::Loading)
		{
			return m_task;
		}
		return nullptr;
	}

	void RuntimeCatalog::load()
	{
		if (m_state != State::Loading)
		{
			m_state = State::Loading;
			m_task.reset(new RuntimeCatalogTask(this, m_scope));
			m_task->start();
		}
	}

	const BaseVersion::Ptr RuntimeCatalog::at(int i) const
	{
		return m_entries.at(i);
	}

	bool RuntimeCatalog::isLoaded()
	{
		return m_state == State::Ready;
	}

	int RuntimeCatalog::count() const
	{
		return m_entries.count();
	}

	QVariant RuntimeCatalog::data(const QModelIndex& index, int role) const
	{
		if (!index.isValid())
			return QVariant();

		if (index.row() > count())
			return QVariant();

		auto runtime = std::dynamic_pointer_cast<RuntimeInstall>(m_entries[index.row()]);
		switch (role)
		{
			case SortRole: return -index.row();
			case VersionPointerRole: return QVariant::fromValue(m_entries[index.row()]);
			case VersionIdRole: return runtime->descriptor();
			case VersionRole: return runtime->version.toString();
			case RecommendedRole: return false;
			case PathRole: return runtime->path;
			case CPUArchitectureRole: return runtime->arch;
			default: return QVariant();
		}
	}

	BaseVersionList::RoleList RuntimeCatalog::providesRoles() const
	{
		return { VersionPointerRole, VersionIdRole, VersionRole, RecommendedRole, PathRole, CPUArchitectureRole };
	}

	void RuntimeCatalog::updateListData(QList<BaseVersion::Ptr> versions)
	{
		beginResetModel();
		m_entries = std::move(versions);
		sortVersions();
		endResetModel();
		m_state = State::Ready;
		m_task.reset();
	}

	static bool sortRuntimes(BaseVersion::Ptr left, BaseVersion::Ptr right)
	{
		auto rleft	= std::dynamic_pointer_cast<RuntimeInstall>(right);
		auto rright = std::dynamic_pointer_cast<RuntimeInstall>(left);
		return (*rleft) > (*rright);
	}

	void RuntimeCatalog::sortVersions()
	{
		std::sort(m_entries.begin(), m_entries.end(), sortRuntimes);
	}

	RuntimeCatalogTask::RuntimeCatalogTask(RuntimeCatalog* catalog, RuntimeCatalog::Scope scope)
		: Task(),
		  m_catalog(catalog),
		  m_scope(scope)
	{}

	void RuntimeCatalogTask::executeTask()
	{
		setStatus(tr("Detecting Java installations..."));

		RuntimeScanner scanner;
		QStringList candidatePaths = scanner.collectPaths(m_scope == RuntimeCatalog::Scope::ManagedOnly);

		ConcurrentTask::Ptr job(
			new ConcurrentTask("Runtime detection", APPLICATION->settings()->get("NumberOfConcurrentTasks").toInt()));
		m_job.reset(job);
		connect(m_job.get(), &Task::finished, this, &RuntimeCatalogTask::probeFinished);
		connect(m_job.get(), &Task::progress, this, &Task::setProgress);

		qDebug() << "Probing the following runtime paths:";
		int token = 0;
		for (const auto& candidate : candidatePaths)
		{
			RuntimeProbeTask::ProbeSettings settings;
			settings.binaryPath = candidate;
			settings.token		= token;
			auto probe			= new RuntimeProbeTask(settings);
			connect(probe,
					&RuntimeProbeTask::probeFinished,
					this,
					[this](const RuntimeProbeTask::ProbeReport& report) { m_results << report; });
			job->addTask(Task::Ptr(probe));
			token++;
		}

		m_job->start();
	}

	void RuntimeCatalogTask::probeFinished()
	{
		QList<RuntimeInstallPtr> candidates;
		std::sort(m_results.begin(),
				  m_results.end(),
				  [](const RuntimeProbeTask::ProbeReport& a, const RuntimeProbeTask::ProbeReport& b)
				  { return a.token < b.token; });

		qDebug() << "Found the following valid Java installations:";
		for (const auto& result : m_results)
		{
			if (result.status == RuntimeProbeTask::ProbeReport::Status::Valid)
			{
				RuntimeInstallPtr runtime(new RuntimeInstall());
				runtime->version  = result.version;
				runtime->arch	  = result.platformArch;
				runtime->path	  = result.path;
				runtime->vendor	  = result.vendor;
				runtime->is_64bit = result.is_64bit;
				runtime->managed  = (m_scope == RuntimeCatalog::Scope::ManagedOnly);
				candidates.append(runtime);

				qDebug() << " " << runtime->version.toString() << runtime->arch << runtime->path;
			}
		}

		QList<BaseVersion::Ptr> entries;
		for (const auto& runtime : candidates)
		{
			BaseVersion::Ptr base = std::dynamic_pointer_cast<BaseVersion>(runtime);
			if (base)
			{
				entries.append(runtime);
			}
		}

		m_catalog->updateListData(entries);
		emitSucceeded();
	}
} // namespace projt::java
