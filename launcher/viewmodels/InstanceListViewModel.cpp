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

#include "InstanceListViewModel.h"

#include "Application.h"
#include "BaseInstance.h"
#include "InstanceList.h"

InstanceListViewModel::InstanceListViewModel(QObject* parent) : QObject(parent) {}

int InstanceListViewModel::totalCount() const
{
    return m_totalCount;
}

QString InstanceListViewModel::selectedInstanceId() const
{
    return m_selectedInstanceId;
}

bool InstanceListViewModel::isBusy() const
{
    return m_busy;
}

QString InstanceListViewModel::busyReason() const
{
    return m_busyReason;
}

QStringList InstanceListViewModel::instanceNames() const
{
    return m_instanceNames;
}

QStringList InstanceListViewModel::instanceIds() const
{
    return m_instanceIds;
}

QStringList InstanceListViewModel::instanceIcons() const
{
    return m_instanceIcons;
}

QStringList InstanceListViewModel::instanceGroups() const
{
    return m_instanceGroups;
}

void InstanceListViewModel::setTotalCount(int count)
{
    if (m_totalCount == count) {
        return;
    }
    m_totalCount = count;
    emit totalCountChanged();
}

void InstanceListViewModel::setSelectedInstanceId(const QString& id)
{
    if (m_selectedInstanceId == id) {
        return;
    }
    m_selectedInstanceId = id;
    emit selectedInstanceIdChanged();
}

void InstanceListViewModel::setBusy(bool busy, const QString& reason)
{
    const bool busyStateChanged = m_busy != busy;
    bool reasonChanged = false;

    if (busy) {
        if (m_busyReason != reason) {
            m_busyReason = reason;
            reasonChanged = true;
        }
    } else if (!m_busyReason.isEmpty()) {
        m_busyReason.clear();
        reasonChanged = true;
    }

    if (busyStateChanged) {
        m_busy = busy;
    }

    if (busyStateChanged || reasonChanged) {
        emit busyChanged();
    }

    if (busyStateChanged) {
        if (m_busy) {
            emit started();
        } else {
            emit finished();
        }
    }
}

void InstanceListViewModel::setInstanceNames(const QStringList& names)
{
    if (m_instanceNames == names) {
        return;
    }
    m_instanceNames = names;
    emit instanceNamesChanged();
}

void InstanceListViewModel::setInstanceIds(const QStringList& ids)
{
    if (m_instanceIds == ids) {
        return;
    }
    m_instanceIds = ids;
    emit instanceListChanged();
}

void InstanceListViewModel::setInstanceIcons(const QStringList& icons)
{
    if (m_instanceIcons == icons) {
        return;
    }
    m_instanceIcons = icons;
    emit instanceListChanged();
}

void InstanceListViewModel::setInstanceGroups(const QStringList& groups)
{
    if (m_instanceGroups == groups) {
        return;
    }
    m_instanceGroups = groups;
    emit instanceListChanged();
}

void InstanceListViewModel::setInstanceLists(const QStringList& ids, const QStringList& names, const QStringList& icons,
                                             const QStringList& groups)
{
    m_instanceIds = ids;
    m_instanceNames = names;
    m_instanceIcons = icons;
    m_instanceGroups = groups;
    emit instanceListChanged();
    emit instanceNamesChanged();
}

void InstanceListViewModel::selectInstance(const QString& id)
{
    setSelectedInstanceId(id);
    emit instanceSelected(id);
}

void InstanceListViewModel::selectInstanceByIndex(int index)
{
    if (index < 0 || index >= m_instanceIds.size()) {
        return;
    }
    selectInstance(m_instanceIds.value(index));
}

void InstanceListViewModel::selectInstanceById(const QString& id)
{
    selectInstance(id);
}

void InstanceListViewModel::launchSelectedInstance()
{
    launchInstance(m_selectedInstanceId);
}

void InstanceListViewModel::launchInstance(const QString& id)
{
    if (id.isEmpty()) {
        emit errorOccurred(tr("No instance selected."));
        return;
    }
    auto instance = resolveInstance(id);
    if (!instance) {
        emit errorOccurred(tr("The selected instance could not be found."));
        return;
    }
    if (instance->isRunning()) {
        emit errorOccurred(tr("The selected instance is already running."));
        return;
    }
    APPLICATION->launch(instance);
}

void InstanceListViewModel::deleteSelectedInstance()
{
    deleteInstance(m_selectedInstanceId);
}

void InstanceListViewModel::deleteInstance(const QString& id)
{
    if (id.isEmpty()) {
        emit errorOccurred(tr("No instance selected."));
        return;
    }
    auto instances = instanceList();
    if (!instances) {
        emit errorOccurred(tr("Instance list is not available."));
        return;
    }

    setBusy(true, tr("Deleting instance"));

    const bool trashed = instances->trashInstance(id);
    if (!trashed) {
        instances->deleteInstance(id);
    }

    if (m_selectedInstanceId == id) {
        setSelectedInstanceId(QString());
    }

    setBusy(false);
}

void InstanceListViewModel::refreshInstances()
{
    auto instances = instanceList();
    if (!instances) {
        emit errorOccurred(tr("Instance list is not available."));
        return;
    }
    setBusy(true, tr("Refreshing instances"));
    instances->loadList();
    setBusy(false);
}

void InstanceListViewModel::renameSelectedInstance(const QString& newName)
{
    if (m_selectedInstanceId.isEmpty() || newName.isEmpty()) {
        return;
    }
    emit renameRequested(m_selectedInstanceId, newName);
}

void InstanceListViewModel::duplicateSelectedInstance(const QString& newIdOrName)
{
    if (m_selectedInstanceId.isEmpty() || newIdOrName.isEmpty()) {
        return;
    }
    emit duplicateRequested(m_selectedInstanceId, newIdOrName);
}

InstanceList* InstanceListViewModel::instanceList() const
{
    auto list = APPLICATION->instances();
    return list ? list.get() : nullptr;
}

std::shared_ptr<BaseInstance> InstanceListViewModel::resolveInstance(const QString& id) const
{
    auto list = APPLICATION->instances();
    if (!list) {
        return {};
    }
    return list->getInstanceById(id);
}
