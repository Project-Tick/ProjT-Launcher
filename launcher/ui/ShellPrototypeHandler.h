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

#include <QDockWidget>

#include "settings/SettingsObject.h"

class LauncherViewModel;
class InstanceListViewModel;
class NewsViewModel;
class SettingsViewModel;
class ShellStateBridge;
class QQuickWidget;

class ShellPrototypeHandler : public QDockWidget {
    Q_OBJECT

   public:
    ShellPrototypeHandler(LauncherViewModel* launcherViewModel,
                          InstanceListViewModel* instanceListViewModel,
                          NewsViewModel* newsViewModel,
                          SettingsViewModel* settingsViewModel,
                          QWidget* parent = nullptr);

   private:
    void exposeContextProperties(LauncherViewModel* launcherViewModel,
                                 InstanceListViewModel* instanceListViewModel,
                                 NewsViewModel* newsViewModel,
                                 SettingsViewModel* settingsViewModel,
                                 SettingsObjectPtr settings);

    QQuickWidget* m_quickWidget = nullptr;
    ShellStateBridge* m_stateBridge = nullptr;
};
