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

#include "modplatform/CheckUpdateTask.h"

class ModrinthCheckUpdate : public CheckUpdateTask
{
	Q_OBJECT

  public:
	ModrinthCheckUpdate(QList<Resource*>& resources,
						std::list<Version>& mcVersions,
						QList<ModPlatform::ModLoaderType> loadersList,
						std::shared_ptr<ResourceFolderModel> resourceModel);

  public slots:
	bool abort() override;

  protected slots:
	void executeTask() override;
	void getUpdateModsForLoader(std::optional<ModPlatform::ModLoaderTypes> loader = {},
								bool forceModLoaderCheck						  = false);
	void checkVersionsResponse(std::shared_ptr<QByteArray> response, std::optional<ModPlatform::ModLoaderTypes> loader);
	void checkNextLoader();

  private:
	Task::Ptr m_job = nullptr;
	QHash<QString, Resource*> m_mappings;
	QString m_hashType;
	int m_loaderIdx	  = 0;
	int m_initialSize = 0;
};
