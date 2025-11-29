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

#include "LogsViewModel.h"

LogsViewModel::LogsViewModel(QObject* parent) : QObject(parent) {}

QString LogsViewModel::logText() const
{
    return m_logText;
}

QString LogsViewModel::category() const
{
    return m_category;
}

bool LogsViewModel::isBusy() const
{
    return m_busy;
}

void LogsViewModel::setLogText(const QString& text)
{
    if (m_logText == text) {
        return;
    }
    m_logText = text;
    emit logTextChanged();
    emit logsUpdated();
}

void LogsViewModel::setCategory(const QString& category)
{
    if (m_category == category) {
        return;
    }
    m_category = category;
    emit categoryChanged();
}

void LogsViewModel::setBusy(bool busy)
{
    if (m_busy == busy) {
        return;
    }
    m_busy = busy;
    emit busyChanged();
}

void LogsViewModel::requestRefresh()
{
    emit refreshRequested(m_category);
}
