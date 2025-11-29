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

void InstanceListViewModel::setBusy(bool busy)
{
    if (m_busy == busy) {
        return;
    }
    m_busy = busy;
    emit busyChanged();
}
