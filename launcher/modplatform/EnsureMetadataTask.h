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

#include "ModIndex.h"
#include "net/NetJob.h"

#include "modplatform/helpers/HashUtils.h"

#include "minecraft/mod/Resource.hpp"
#include "tasks/ConcurrentTask.h"

class Mod;
class QDir;

class EnsureMetadataTask : public Task
{
	Q_OBJECT

  public:
	EnsureMetadataTask(Resource*, QDir, ModPlatform::ResourceProvider = ModPlatform::ResourceProvider::MODRINTH);
	EnsureMetadataTask(QList<Resource*>&,
					   QDir,
					   ModPlatform::ResourceProvider = ModPlatform::ResourceProvider::MODRINTH);
	EnsureMetadataTask(QHash<QString, Resource*>&,
					   QDir,
					   ModPlatform::ResourceProvider = ModPlatform::ResourceProvider::MODRINTH);

	~EnsureMetadataTask() = default;

	Task::Ptr getHashingTask()
	{
		return m_hashingTask;
	}

  public slots:
	bool abort() override;
  protected slots:
	void executeTask() override;

  private:
	// Platform-specific version/project fetching (kept together for consistency)
	Task::Ptr modrinthVersionsTask();
	Task::Ptr modrinthProjectsTask();

	Task::Ptr flameVersionsTask();
	Task::Ptr flameProjectsTask();

	// Helpers
	enum class RemoveFromList
	{
		Yes,
		No
	};
	void emitReady(Resource*, QString key = {}, RemoveFromList = RemoveFromList::Yes);
	void emitFail(Resource*, QString key = {}, RemoveFromList = RemoveFromList::Yes);

	// Hashes and stuff
	Hashing::Hasher::Ptr createNewHash(Resource*);
	QString getExistingHash(Resource*);

  private slots:
	void updateMetadata(ModPlatform::IndexedPack& pack, ModPlatform::IndexedVersion& ver, Resource*);
	void updateMetadataCallback(ModPlatform::IndexedPack& pack, Resource* resource);

  signals:
	void metadataReady(Resource*);
	void metadataFailed(Resource*);

  private:
	QHash<QString, Resource*> m_resources;
	QDir m_indexDir;
	ModPlatform::ResourceProvider m_provider;

	QHash<QString, ModPlatform::IndexedVersion> m_tempVersions;
	Task::Ptr m_hashingTask;
	Task::Ptr m_currentTask;
	QHash<QString, Task::Ptr> m_updateMetadataTasks;
};
