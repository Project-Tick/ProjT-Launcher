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

class ModrinthCheckUpdate : public CheckUpdateTask {
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
    void getUpdateModsForLoader(std::optional<ModPlatform::ModLoaderTypes> loader = {}, bool forceModLoaderCheck = false);
    void checkVersionsResponse(std::shared_ptr<QByteArray> response, std::optional<ModPlatform::ModLoaderTypes> loader);
    void checkNextLoader();

   private:
    Task::Ptr m_job = nullptr;
    QHash<QString, Resource*> m_mappings;
    QString m_hashType;
    int m_loaderIdx = 0;
    int m_initialSize = 0;
};
