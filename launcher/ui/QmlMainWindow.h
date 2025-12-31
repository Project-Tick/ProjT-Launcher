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
 *
 */

#pragma once

#include <QCloseEvent>
#include <QMainWindow>

#include "settings/SettingsObject.h"

class LauncherViewModel;
class InstanceListViewModel;
class NewsViewModel;
class SettingsViewModel;
class ThemeViewModel;
class ShellStateBridge;
class ThemeViewModel;
class ShellStateBridge;
class QQuickView;

class QmlMainWindow : public QMainWindow {
    Q_OBJECT

   public:
    QmlMainWindow(LauncherViewModel* launcherViewModel,
                  InstanceListViewModel* instanceListViewModel,
                  NewsViewModel* newsViewModel,
                  SettingsViewModel* settingsViewModel,
                  ThemeViewModel* themeViewModel,
                  QWidget* parent = nullptr);

    void openNewInstanceDialog(const QString& groupName, const QString& importUrl);
    void openSettingsPage(const QString& pageKey);
    void openInstanceSettingsPage(const QString& instanceId, const QString& pageKey);
    void openSetupWizard(const QStringList& pageIds);

    // Process URLs for importing modpacks, instances, etc.
    void processURLs(const QList<QUrl>& urls);

   protected:
    void closeEvent(QCloseEvent* event) override;

   private:
    void exposeContextProperties(LauncherViewModel* launcherViewModel,
                                 InstanceListViewModel* instanceListViewModel,
                                 NewsViewModel* newsViewModel,
                                 SettingsViewModel* settingsViewModel,
                                 ThemeViewModel* themeViewModel,
                                 SettingsObjectPtr settings);

    QQuickView* m_quickView = nullptr;
    ShellStateBridge* m_stateBridge = nullptr;
};
