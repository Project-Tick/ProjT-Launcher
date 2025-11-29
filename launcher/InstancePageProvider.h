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

#include <FileSystem.h>
#include <ui/pages/instance/DataPackPage.h>
#include "minecraft/MinecraftInstance.h"
#include "ui/pages/BasePage.h"
#include "ui/pages/BasePageProvider.h"
#include "ui/pages/instance/BackupPage.h"
#include "ui/pages/instance/InstanceSettingsPage.h"
#include "ui/pages/instance/LogPage.h"
#include "ui/pages/instance/ManagedPackPage.h"
#include "ui/pages/instance/ModFolderPage.h"
#include "ui/pages/instance/NotesPage.h"
#include "ui/pages/instance/OtherLogsPage.h"
#include "ui/pages/instance/ResourcePackPage.h"
#include "ui/pages/instance/ScreenshotsPage.h"
#include "ui/pages/instance/ServersPage.h"
#include "ui/pages/instance/ShaderPackPage.h"
#include "ui/pages/instance/TexturePackPage.h"
#include "ui/pages/instance/VersionPage.h"
#include "ui/pages/instance/WorldListPage.h"
#include "viewmodels/InstanceListViewModel.h"
#include "viewmodels/SettingsViewModel.h"

class InstancePageProvider : protected QObject, public BasePageProvider {
    Q_OBJECT
   public:
    explicit InstancePageProvider(InstancePtr parent, InstanceListViewModel* viewModel, SettingsViewModel* settingsViewModel)
        : inst(parent)
        , m_instanceViewModel(viewModel)
        , m_settingsViewModel(settingsViewModel)
    {}

    virtual ~InstancePageProvider() = default;
    virtual QList<BasePage*> getPages() override
    {
        QList<BasePage*> values;
        values.append(new LogPage(inst));
        std::shared_ptr<MinecraftInstance> onesix = std::dynamic_pointer_cast<MinecraftInstance>(inst);
        values.append(new VersionPage(onesix.get()));
        values.append(ManagedPackPage::createPage(onesix.get()));
        auto modsPage = new ModFolderPage(onesix.get(), onesix->loaderModList());
        modsPage->setFilter("%1 (*.zip *.jar *.litemod *.nilmod)");
        values.append(modsPage);
        values.append(new CoreModFolderPage(onesix.get(), onesix->coreModList()));
        values.append(new NilModFolderPage(onesix.get(), onesix->nilModList()));
        values.append(new ResourcePackPage(onesix.get(), onesix->resourcePackList()));
        values.append(new GlobalDataPackPage(onesix.get()));
        values.append(new TexturePackPage(onesix.get(), onesix->texturePackList()));
        values.append(new ShaderPackPage(onesix.get(), onesix->shaderPackList()));
        values.append(new NotesPage(onesix.get(), m_instanceViewModel));
        values.append(new WorldListPage(onesix, onesix->worldList()));
        values.append(new ServersPage(onesix));
        values.append(new ScreenshotsPage(FS::PathCombine(onesix->gameRoot(), "screenshots")));
        values.append(new BackupPage(onesix.get()));
        values.append(new InstanceSettingsPage(onesix, m_settingsViewModel));
        values.append(new OtherLogsPage("logs", tr("Other Logs"), "Other-Logs", inst));
        return values;
    }

    virtual QString dialogTitle() override { return tr("Edit Instance (%1)").arg(inst->name()); }

   protected:
    InstancePtr inst;
    InstanceListViewModel* m_instanceViewModel = nullptr;
    SettingsViewModel* m_settingsViewModel = nullptr;
};
