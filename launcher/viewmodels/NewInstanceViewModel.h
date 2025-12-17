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

#include <QAbstractListModel>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantList>
#include <QVariantMap>

#include "BaseVersionList.h"
#include "InstanceTask.h"
#include "QObjectPtr.h"
#include "tasks/Task.h"

class BaseVersionList;

class NewInstanceViewModel : public QObject {
    Q_OBJECT

    // Instance Info
    Q_PROPERTY(QString instanceName READ instanceName WRITE setInstanceName NOTIFY instanceNameChanged)
    Q_PROPERTY(QString instanceGroup READ instanceGroup WRITE setInstanceGroup NOTIFY instanceGroupChanged)
    Q_PROPERTY(QString iconKey READ iconKey WRITE setIconKey NOTIFY iconKeyChanged)

    // Minecraft Version
    Q_PROPERTY(QString selectedMinecraftVersion READ selectedMinecraftVersion WRITE setSelectedMinecraftVersion NOTIFY
                   selectedMinecraftVersionChanged)
    Q_PROPERTY(bool showReleases READ showReleases WRITE setShowReleases NOTIFY showReleasesChanged)
    Q_PROPERTY(bool showSnapshots READ showSnapshots WRITE setShowSnapshots NOTIFY showSnapshotsChanged)
    Q_PROPERTY(bool showOldVersions READ showOldVersions WRITE setShowOldVersions NOTIFY showOldVersionsChanged)
    Q_PROPERTY(bool showExperiments READ showExperiments WRITE setShowExperiments NOTIFY showExperimentsChanged)

    // Mod Loader
    Q_PROPERTY(QString selectedModLoader READ selectedModLoader WRITE setSelectedModLoader NOTIFY selectedModLoaderChanged)
    Q_PROPERTY(QString selectedModLoaderVersion READ selectedModLoaderVersion WRITE setSelectedModLoaderVersion NOTIFY
                   selectedModLoaderVersionChanged)
    Q_PROPERTY(QStringList availableModLoaders READ availableModLoaders NOTIFY availableModLoadersChanged)

    // Models
    Q_PROPERTY(QAbstractListModel* minecraftVersionsModel READ minecraftVersionsModel NOTIFY minecraftVersionsModelChanged)
    Q_PROPERTY(QAbstractListModel* modLoaderVersionsModel READ modLoaderVersionsModel NOTIFY modLoaderVersionsModelChanged)
    Q_PROPERTY(QStringList groupList READ groupList NOTIFY groupListChanged)

    // State
    Q_PROPERTY(bool isLoading READ isLoading NOTIFY isLoadingChanged)
    Q_PROPERTY(bool isValid READ isValid NOTIFY isValidChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusMessageChanged)

   public:
    explicit NewInstanceViewModel(QObject* parent = nullptr);
    ~NewInstanceViewModel();

    // Properties getters
    QString instanceName() const;
    QString instanceGroup() const;
    QString iconKey() const;
    QString selectedMinecraftVersion() const;
    bool showReleases() const;
    bool showSnapshots() const;
    bool showOldVersions() const;
    bool showExperiments() const;
    QString selectedModLoader() const;
    QString selectedModLoaderVersion() const;
    QStringList availableModLoaders() const;
    QAbstractListModel* minecraftVersionsModel() const;
    QAbstractListModel* modLoaderVersionsModel() const;
    QStringList groupList() const;
    bool isLoading() const;
    bool isValid() const;
    QString statusMessage() const;

    // Properties setters
    void setInstanceName(const QString& name);
    void setInstanceGroup(const QString& group);
    void setIconKey(const QString& key);
    void setSelectedMinecraftVersion(const QString& version);
    void setShowReleases(bool show);
    void setShowSnapshots(bool show);
    void setShowOldVersions(bool show);
    void setShowExperiments(bool show);
    void setSelectedModLoader(const QString& loader);
    void setSelectedModLoaderVersion(const QString& version);

    // Actions
    Q_INVOKABLE void loadMinecraftVersions();
    Q_INVOKABLE void loadModLoaderVersions();
    Q_INVOKABLE void refreshVersionLists();
    Q_INVOKABLE void filterVersions();
    Q_INVOKABLE void createInstance();
    Q_INVOKABLE void cancel();
    Q_INVOKABLE void reset();

    // Utility
    Q_INVOKABLE QVariantMap getMinecraftVersionInfo(int index) const;
    Q_INVOKABLE QVariantMap getModLoaderVersionInfo(int index) const;
    Q_INVOKABLE QString suggestInstanceName() const;
    Q_INVOKABLE bool isModLoaderCompatible(const QString& loader, const QString& mcVersion) const;

   signals:
    void instanceNameChanged();
    void instanceGroupChanged();
    void iconKeyChanged();
    void selectedMinecraftVersionChanged();
    void showReleasesChanged();
    void showSnapshotsChanged();
    void showOldVersionsChanged();
    void showExperimentsChanged();
    void selectedModLoaderChanged();
    void selectedModLoaderVersionChanged();
    void availableModLoadersChanged();
    void minecraftVersionsModelChanged();
    void modLoaderVersionsModelChanged();
    void groupListChanged();
    void isLoadingChanged();
    void isValidChanged();
    void statusMessageChanged();

    void instanceCreationStarted();
    void instanceCreationFinished(bool success, const QString& message);
    void instanceCreationProgress(qint64 current, qint64 total);

   private slots:
    void onMinecraftVersionsLoaded();
    void onModLoaderVersionsLoaded();
    void onInstanceCreationSucceeded();
    void onInstanceCreationFailed(const QString& reason);
    void onInstanceCreationProgress(qint64 current, qint64 total);

   private:
    void updateValidity();
    void loadGroups();
    QString modLoaderUid(const QString& loader) const;

    // Instance info
    QString m_instanceName;
    QString m_instanceGroup;
    QString m_iconKey = "default";

    // Version selection
    QString m_selectedMinecraftVersion;
    bool m_showReleases = true;
    bool m_showSnapshots = false;
    bool m_showOldVersions = false;
    bool m_showExperiments = false;

    // Mod loader
    QString m_selectedModLoader;
    QString m_selectedModLoaderVersion;
    QStringList m_availableModLoaders;

    // Models
    BaseVersionList* m_minecraftVersionsModel = nullptr;
    BaseVersionList* m_modLoaderVersionsModel = nullptr;
    QStringList m_groupList;

    // State
    bool m_isLoading = false;
    bool m_isValid = false;
    QString m_statusMessage;

    // Tasks
    Task::Ptr m_loadTask;
    InstanceTask* m_creationTask = nullptr;
};
