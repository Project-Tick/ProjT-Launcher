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
 * === Upstream License Block (Do Not Modify) ==============================
 *
 *
 *
 *
 *
 * ======================================================================== */

#include "TexturePackModel.h"

#include "Application.h"

#include "meta/Index.hpp"
#include "meta/Version.hpp"

static std::list<Version> s_availableVersions = {};

namespace ResourceDownload
{
	TexturePackResourceModel::TexturePackResourceModel(BaseInstance const& inst,
													   ResourceAPI* api,
													   QString debugName,
													   QString metaEntryBase)
		: ResourcePackResourceModel(inst, api, debugName, metaEntryBase),
		  m_version_list(APPLICATION->metadataIndex()->component("net.minecraft"))
	{
		if (!m_version_list->isLoaded())
		{
			qDebug() << "Loading version list...";
			m_task = m_version_list->getLoadTask();
			if (!m_task->isRunning())
				m_task->start();
		}
	}

	void waitOnVersionListLoad(projt::meta::MetaVersionList::Ptr version_list)
	{
		QEventLoop load_version_list_loop;

		QTimer time_limit_for_list_load;
		time_limit_for_list_load.setTimerType(Qt::TimerType::CoarseTimer);
		time_limit_for_list_load.setSingleShot(true);
		time_limit_for_list_load.callOnTimeout(&load_version_list_loop, &QEventLoop::quit);
		time_limit_for_list_load.start(4000);

		auto task = version_list->getLoadTask();
		QObject::connect(task.get(), &Task::finished, &load_version_list_loop, &QEventLoop::quit);
		if (!task->isRunning())
			task->start();
		load_version_list_loop.exec();
		if (time_limit_for_list_load.isActive())
			time_limit_for_list_load.stop();
	}

	ResourceAPI::SearchArgs TexturePackResourceModel::createSearchArguments()
	{
		if (s_availableVersions.empty())
			waitOnVersionListLoad(m_version_list);

		auto args = ResourcePackResourceModel::createSearchArguments();

		if (!m_version_list->isLoaded())
		{
			qCritical() << "The version list could not be loaded. Falling back to showing all entries.";
			return args;
		}

		if (s_availableVersions.empty())
		{
			for (auto&& version : m_version_list->allVersions())
			{
				// NOTE: Logic duplicated from meta 'texturepacks' trait due to lack of index file access.
				//        Avoiding download of every version file for check.
				if (auto ver = version->asComparableVersion(); ver <= maximumTexturePackVersion())
					s_availableVersions.push_back(ver);
			}
		}

		Q_ASSERT(!s_availableVersions.empty());

		args.versions = s_availableVersions;

		return args;
	}

	ResourceAPI::VersionSearchArgs TexturePackResourceModel::createVersionsArguments(const QModelIndex& entry)
	{
		auto args		  = ResourcePackResourceModel::createVersionsArguments(entry);
		args.resourceType = ModPlatform::ResourceType::TexturePack;
		if (!m_version_list->isLoaded())
		{
			qCritical() << "The version list could not be loaded. Falling back to showing all entries.";
			return args;
		}

		args.mcVersions = s_availableVersions;
		return args;
	}

} // namespace ResourceDownload
