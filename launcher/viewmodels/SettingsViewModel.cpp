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

#include "SettingsViewModel.h"

SettingsViewModel::SettingsViewModel(QObject* parent) : QObject(parent) {}

QString SettingsViewModel::instanceId() const
{
    return m_instanceId;
}

QString SettingsViewModel::currentCategory() const
{
    return m_currentCategory;
}

bool SettingsViewModel::isBusy() const
{
    return m_busy;
}

void SettingsViewModel::setInstanceId(const QString& id)
{
    if (m_instanceId == id) {
        return;
    }
    m_instanceId = id;
    emit instanceIdChanged();
}

void SettingsViewModel::setCurrentCategory(const QString& category)
{
    if (m_currentCategory == category) {
        return;
    }
    m_currentCategory = category;
    emit currentCategoryChanged();
}

void SettingsViewModel::setBusy(bool busy)
{
    if (m_busy == busy) {
        return;
    }
    m_busy = busy;
    emit busyChanged();
}

void SettingsViewModel::notifySettingsLoaded()
{
    emit settingsLoaded();
}

void SettingsViewModel::notifySettingsChanged()
{
    emit settingsChanged();
}

void SettingsViewModel::notifySaveRequested()
{
    emit saveRequested();
}
