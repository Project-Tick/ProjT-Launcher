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

#include "BaseInstance.h"
#include "ResourceDownloadTask.h"
#include "ReviewMessageBox.h"

#include "minecraft/mod/ModFolderModel.h"

#include "modplatform/CheckUpdateTask.h"

class Mod;
class ModrinthCheckUpdate;
class FlameCheckUpdate;
class ConcurrentTask;

class ResourceUpdateDialog final : public ReviewMessageBox {
    Q_OBJECT
   public:
    explicit ResourceUpdateDialog(QWidget* parent,
                                  BaseInstance* instance,
                                  std::shared_ptr<ResourceFolderModel> resourceModel,
                                  QList<Resource*>& searchFor,
                                  bool includeDeps,
                                  QList<ModPlatform::ModLoaderType> loadersList = {});

    void checkCandidates();

    void appendResource(const CheckUpdateTask::Update& info, QStringList requiredBy = {});

    const QList<ResourceDownloadTask::Ptr> getTasks();
    auto indexDir() const -> QDir { return m_resourceModel->indexDir(); }

    auto noUpdates() const -> bool { return m_noUpdates; };
    auto aborted() const -> bool { return m_aborted; };

   private:
    auto ensureMetadata() -> bool;

   private slots:
    void onMetadataEnsured(Resource* resource);
    void onMetadataFailed(Resource* resource,
                          bool try_others = false,
                          ModPlatform::ResourceProvider firstChoice = ModPlatform::ResourceProvider::MODRINTH);

   private:
    QWidget* m_parent;

    shared_qobject_ptr<ModrinthCheckUpdate> m_modrinthCheckTask;
    shared_qobject_ptr<FlameCheckUpdate> m_flameCheckTask;

    const std::shared_ptr<ResourceFolderModel> m_resourceModel;

    QList<Resource*>& m_candidates;
    QList<Resource*> m_modrinthToUpdate;
    QList<Resource*> m_flameToUpdate;

    ConcurrentTask::Ptr m_secondTryMetadata;
    QList<std::tuple<Resource*, QString>> m_failedMetadata;
    QList<std::tuple<Resource*, QString, QUrl>> m_failedCheckUpdate;

    QHash<QString, ResourceDownloadTask::Ptr> m_tasks;
    BaseInstance* m_instance;

    bool m_noUpdates = false;
    bool m_aborted = false;
    bool m_includeDeps = false;
    QList<ModPlatform::ModLoaderType> m_loadersList;
};
