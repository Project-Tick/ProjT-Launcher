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

#include "modplatform/CheckUpdateTask.h"

class FlameCheckUpdate : public CheckUpdateTask {
    Q_OBJECT

   public:
    FlameCheckUpdate(QList<Resource*>& resources,
                     std::list<Version>& mcVersions,
                     QList<ModPlatform::ModLoaderType> loadersList,
                     std::shared_ptr<ResourceFolderModel> resourceModel)
        : CheckUpdateTask(resources, mcVersions, std::move(loadersList), std::move(resourceModel))
    {}

   public slots:
    bool abort() override;

   protected slots:
    void executeTask() override;
   private slots:
    void getLatestVersionCallback(Resource* resource, std::shared_ptr<QByteArray> response);
    void collectBlockedMods();

   private:
    Task::Ptr m_task = nullptr;

    QHash<Resource*, QString> m_blocked;
};
