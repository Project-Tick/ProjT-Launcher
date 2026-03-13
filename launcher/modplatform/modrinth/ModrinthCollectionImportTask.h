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

#pragma once

#include "Version.h"
#include "modplatform/ModIndex.h"
#include "modplatform/modrinth/ModrinthAPI.h"
#include "tasks/Task.h"

#include <QList>
#include <QString>

#include <list>

class MinecraftInstance;

class ModrinthCollectionImportTask final : public Task
{
	Q_OBJECT

  public:
	struct ImportedResource
	{
		ModPlatform::IndexedPack::Ptr pack;
		ModPlatform::IndexedVersion version;
	};

	explicit ModrinthCollectionImportTask(QString collection_reference, MinecraftInstance* instance);

	bool abort() override;

	QString collectionName() const
	{
		return m_collection_name;
	}

	QList<ImportedResource> importedResources() const
	{
		return m_imported_resources;
	}

	QStringList skippedResources() const
	{
		return m_skipped_resources;
	}

  protected:
	void executeTask() override;

  private:
	QString normalizeCollectionReference(QString reference) const;
	QString collectionUrl() const;
	bool extractCollectionMetadata(const QByteArray& html);
	void fetchCollectionPage();
	void fetchProjectBatch();
	void fetchNextVersion();
	void finishImport();

  private:
	QString m_collection_reference;
	QString m_collection_name;
	MinecraftInstance* m_instance = nullptr;

	std::list<Version> m_mc_versions;
	ModPlatform::ModLoaderTypes m_loaders = {};

	QStringList m_project_ids;
	int m_project_batch_offset = 0;
	int m_version_index		   = 0;

	QList<ModPlatform::IndexedPack::Ptr> m_packs;
	QList<ImportedResource> m_imported_resources;
	QStringList m_skipped_resources;

	ModrinthAPI m_api;
	shared_qobject_ptr<Task> m_current_task;
};
