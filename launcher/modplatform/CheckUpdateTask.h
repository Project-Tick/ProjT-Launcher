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
#pragma once

#include "minecraft/mod/tasks/GetModDependenciesTask.hpp"
#include "modplatform/ModIndex.h"
#include "tasks/Task.h"

class ResourceDownloadTask;
class ModFolderModel;

class CheckUpdateTask : public Task
{
	Q_OBJECT

  public:
	CheckUpdateTask(QList<Resource*>& resources,
					std::list<Version>& mcVersions,
					QList<ModPlatform::ModLoaderType> loadersList,
					std::shared_ptr<ResourceFolderModel> resourceModel)
		: Task(),
		  m_resources(resources),
		  m_gameVersions(mcVersions),
		  m_loadersList(std::move(loadersList)),
		  m_resourceModel(std::move(resourceModel))
	{}

	struct Update
	{
		QString name;
		QString old_hash;
		QString old_version;
		QString new_version;
		std::optional<ModPlatform::IndexedVersionType> new_version_type;
		QString changelog;
		ModPlatform::ResourceProvider provider;
		shared_qobject_ptr<ResourceDownloadTask> download;
		bool enabled = true;

	  public:
		Update(QString name,
			   QString old_h,
			   QString old_v,
			   QString new_v,
			   std::optional<ModPlatform::IndexedVersionType> new_v_type,
			   QString changelog,
			   ModPlatform::ResourceProvider p,
			   shared_qobject_ptr<ResourceDownloadTask> t,
			   bool enabled = true)
			: name(std::move(name)),
			  old_hash(std::move(old_h)),
			  old_version(std::move(old_v)),
			  new_version(std::move(new_v)),
			  new_version_type(std::move(new_v_type)),
			  changelog(std::move(changelog)),
			  provider(p),
			  download(std::move(t)),
			  enabled(enabled)
		{}
	};

	auto getUpdates() -> std::vector<Update>&&
	{
		return std::move(m_updates);
	}
	auto getDependencies() -> QList<std::shared_ptr<GetModDependenciesTask::PackDependency>>&&
	{
		return std::move(m_deps);
	}

  public slots:
	bool abort() override = 0;

  protected slots:
	void executeTask() override = 0;

  signals:
	void checkFailed(Resource* failed, QString reason, QUrl recover_url = {});

  protected:
	QList<Resource*>& m_resources;
	std::list<Version>& m_gameVersions;
	QList<ModPlatform::ModLoaderType> m_loadersList;
	std::shared_ptr<ResourceFolderModel> m_resourceModel;

	std::vector<Update> m_updates;
	QList<std::shared_ptr<GetModDependenciesTask::PackDependency>> m_deps;
};
