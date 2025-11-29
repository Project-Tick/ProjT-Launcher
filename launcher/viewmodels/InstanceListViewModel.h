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

#include <memory>

#include <QFileInfoList>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariant>

#include "InstanceTask.h"
#include "minecraft/ShortcutUtils.h"
#include "minecraft/BackupManager.h"

class BaseInstance;
class InstanceList;

class InstanceListViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(int totalCount READ totalCount NOTIFY totalCountChanged)
    Q_PROPERTY(QString selectedInstanceId READ selectedInstanceId WRITE setSelectedInstanceId NOTIFY selectedInstanceIdChanged)
    Q_PROPERTY(bool busy READ isBusy WRITE setBusy NOTIFY busyChanged)
    Q_PROPERTY(QString busyReason READ busyReason NOTIFY busyChanged)
    Q_PROPERTY(QStringList instanceNames READ instanceNames NOTIFY instanceNamesChanged)
    Q_PROPERTY(QStringList instanceIds READ instanceIds NOTIFY instanceListChanged)
    Q_PROPERTY(QStringList instanceIcons READ instanceIcons NOTIFY instanceListChanged)
    Q_PROPERTY(QStringList instanceIconPaths READ instanceIconPaths NOTIFY instanceListChanged)
    Q_PROPERTY(QStringList instanceGroups READ instanceGroups NOTIFY instanceListChanged)
    Q_PROPERTY(bool hasSelection READ hasSelection NOTIFY instanceStateChanged)
    Q_PROPERTY(bool canLaunchSelected READ canLaunchSelected NOTIFY instanceStateChanged)
    Q_PROPERTY(bool isSelectedRunning READ isSelectedRunning NOTIFY instanceStateChanged)
    Q_PROPERTY(bool canDeleteSelected READ canDeleteSelected NOTIFY instanceStateChanged)
    Q_PROPERTY(bool canExportSelected READ canExportSelected NOTIFY instanceStateChanged)
    Q_PROPERTY(bool canBackupSelected READ canBackupSelected NOTIFY instanceStateChanged)

   public:
    explicit InstanceListViewModel(QObject* parent = nullptr);

    int totalCount() const;
    QString selectedInstanceId() const;
    bool isBusy() const;
    QString busyReason() const;
    QStringList instanceNames() const;
    QStringList instanceIds() const;
    QStringList instanceIcons() const;
    QStringList instanceIconPaths() const;
    QStringList instanceGroups() const;
    bool hasSelection() const;
    bool canLaunchSelected() const;
    bool isSelectedRunning() const;
    bool canDeleteSelected() const;
    bool canExportSelected() const;
    bool canBackupSelected() const;

    void setTotalCount(int count);
    void setSelectedInstanceId(const QString& id);
    void setBusy(bool busy, const QString& reason = QString());
    void setInstanceNames(const QStringList& names);
    void setInstanceIds(const QStringList& ids);
    void setInstanceIcons(const QStringList& icons);
    void setInstanceGroups(const QStringList& groups);
    void setInstanceLists(const QStringList& ids, const QStringList& names, const QStringList& icons, const QStringList& groups);

    Q_INVOKABLE void selectInstance(const QString& id);
    Q_INVOKABLE void selectInstanceByIndex(int index);
    Q_INVOKABLE void selectInstanceById(const QString& id);
    Q_INVOKABLE void launchSelectedInstance();
    Q_INVOKABLE void launchInstance(const QString& id);
    Q_INVOKABLE void refreshInstances();
    Q_INVOKABLE void deleteSelectedInstance();
    Q_INVOKABLE void deleteInstance(const QString& id);
    Q_INVOKABLE void renameSelectedInstance(const QString& newName);
    Q_INVOKABLE void duplicateSelectedInstance(const QString& newIdOrName);
    Q_INVOKABLE void renameInstance(const QString& id, const QString& newName);
    Q_INVOKABLE void updateInstanceNotes(const QString& id, const QString& notes);
    Q_INVOKABLE void exportInstance(const QString& id, const QString& outputPath, const QFileInfoList& files);
    Q_INVOKABLE void backupInstance(const QString& id, const QString& backupName, const BackupOptions& options);
    Q_INVOKABLE void updateInstanceIcon(const QString& id, const QString& iconKey);
    Q_INVOKABLE void refreshInstanceMetadata(const QString& id);
    Q_INVOKABLE void updateCustomProperty(const QString& id, const QString& key, const QVariant& value);
    Q_INVOKABLE void createGroup(const QString& name);
    Q_INVOKABLE void renameGroup(const QString& oldName, const QString& newName);
    Q_INVOKABLE void deleteGroup(const QString& name);
    Q_INVOKABLE void moveInstanceToGroup(const QString& instanceId, const QString& groupName);
    Q_INVOKABLE bool handleDragDrop(const QString& instanceId, const QString& targetGroup);
    Q_INVOKABLE void reorderInstances(const QString& groupName, const QStringList& orderedInstanceIds);

    void addInstance(InstanceTask* task, const QString& busyReason = QString());
    void copyInstance(InstanceTask* task, const QString& busyReason = QString());
    void createShortcut(const ShortcutUtils::Shortcut& shortcut);

   signals:
    void totalCountChanged();
    void selectedInstanceIdChanged();
    void busyChanged();
    void started();
    void finished();
    void errorOccurred(const QString& message);
    void instanceStateChanged();
    void instanceNamesChanged();
    void instanceListChanged();
    void instanceSelected(const QString& id);
    void renameRequested(const QString& id, const QString& newName);
    void duplicateRequested(const QString& id, const QString& targetIdOrName);

   private:
    InstanceList* instanceList() const;
    std::shared_ptr<BaseInstance> resolveInstance(const QString& id) const;
    void bindInstanceSignals(const std::shared_ptr<BaseInstance>& instance);
    void clearInstanceSignals();
    void refreshInstanceState();
    void startTask(Task* task, const QString& busyReason);
    void startInstanceTask(InstanceTask* task, const QString& busyReason);

    int m_totalCount = 0;
    QString m_selectedInstanceId;
    bool m_busy = false;
    QString m_busyReason;
    std::weak_ptr<BaseInstance> m_currentInstance;
    QMetaObject::Connection m_runningConnection;
    bool m_canLaunchSelected = false;
    bool m_isSelectedRunning = false;
    bool m_canDeleteSelected = false;
    bool m_canExportSelected = false;
    bool m_canBackupSelected = false;
    QStringList m_instanceNames;
    QStringList m_instanceIds;
    QStringList m_instanceIcons;
    QStringList m_instanceIconPaths;
    QStringList m_instanceGroups;
    std::unique_ptr<BackupManager> m_backupManager;
};
