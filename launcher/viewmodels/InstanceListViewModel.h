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

#include <QObject>
#include <QString>
#include <QStringList>

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
    Q_PROPERTY(QStringList instanceGroups READ instanceGroups NOTIFY instanceListChanged)

   public:
    explicit InstanceListViewModel(QObject* parent = nullptr);

    int totalCount() const;
    QString selectedInstanceId() const;
    bool isBusy() const;
    QString busyReason() const;
    QStringList instanceNames() const;
    QStringList instanceIds() const;
    QStringList instanceIcons() const;
    QStringList instanceGroups() const;

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

   signals:
    void totalCountChanged();
    void selectedInstanceIdChanged();
    void busyChanged();
    void started();
    void finished();
    void errorOccurred(const QString& message);
    void instanceNamesChanged();
    void instanceListChanged();
    void instanceSelected(const QString& id);
    void renameRequested(const QString& id, const QString& newName);
    void duplicateRequested(const QString& id, const QString& targetIdOrName);

   private:
    InstanceList* instanceList() const;
    std::shared_ptr<BaseInstance> resolveInstance(const QString& id) const;

    int m_totalCount = 0;
    QString m_selectedInstanceId;
    bool m_busy = false;
    QString m_busyReason;
    QStringList m_instanceNames;
    QStringList m_instanceIds;
    QStringList m_instanceIcons;
    QStringList m_instanceGroups;
};
