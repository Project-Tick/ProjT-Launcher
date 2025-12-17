// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
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
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
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

class ShellManager : public QDockWidget {
    Q_OBJECT

   public:
    ShellManager(LauncherViewModel* launcherViewModel,
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
