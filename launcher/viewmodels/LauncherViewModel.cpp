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

#include "LauncherViewModel.h"

#include "BuildConfig.h"

LauncherViewModel::LauncherViewModel(QObject* parent) : QObject(parent)
{
    m_displayName = BuildConfig.LAUNCHER_DISPLAYNAME;
    m_versionString = BuildConfig.printableVersionString();
}

QString LauncherViewModel::displayName() const
{
    return m_displayName;
}

QString LauncherViewModel::versionString() const
{
    return m_versionString;
}

bool LauncherViewModel::isBusy() const
{
    return m_busy;
}

void LauncherViewModel::setDisplayName(const QString& name)
{
    if (m_displayName == name) {
        return;
    }
    m_displayName = name;
    emit displayNameChanged();
}

void LauncherViewModel::setVersionString(const QString& version)
{
    if (m_versionString == version) {
        return;
    }
    m_versionString = version;
    emit versionStringChanged();
}

void LauncherViewModel::setBusy(bool busy)
{
    if (m_busy == busy) {
        return;
    }
    m_busy = busy;
    emit busyChanged();
}
