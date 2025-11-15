// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  This file is part of ProjT Launcher and is licensed under
 *  the GNU General Public License version 3 or later.
 *
 *  If this file includes work from previous open-source projects,
 *  their original copyright and license notices are preserved below.
 */
#pragma once

#include "ModIndex.h"
#include "net/NetJob.h"

#include "modplatform/helpers/HashUtils.h"

#include "minecraft/mod/Resource.h"
#include "tasks/ConcurrentTask.h"

class Mod;
class QDir;

class EnsureMetadataTask : public Task {
    Q_OBJECT

   public:
    EnsureMetadataTask(Resource*, QDir, ModPlatform::ResourceProvider = ModPlatform::ResourceProvider::MODRINTH);
    EnsureMetadataTask(QList<Resource*>&, QDir, ModPlatform::ResourceProvider = ModPlatform::ResourceProvider::MODRINTH);
    EnsureMetadataTask(QHash<QString, Resource*>&, QDir, ModPlatform::ResourceProvider = ModPlatform::ResourceProvider::MODRINTH);

    ~EnsureMetadataTask() = default;

    Task::Ptr getHashingTask() { return m_hashingTask; }

   public slots:
    bool abort() override;
   protected slots:
    void executeTask() override;

   private:
    // FIXME: Move to their own namespace
    Task::Ptr modrinthVersionsTask();
    Task::Ptr modrinthProjectsTask();

    Task::Ptr flameVersionsTask();
    Task::Ptr flameProjectsTask();

    // Helpers
    enum class RemoveFromList { Yes, No };
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
