// SPDX-License-Identifier: GPL-3.0-only
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

#include <QFileSystemWatcher>
#include <QString>

struct WatchLock {
    WatchLock(QFileSystemWatcher* watcher, const QString& directory) : m_watcher(watcher), m_directory(directory)
    {
        m_watcher->removePath(m_directory);
    }
    ~WatchLock() { m_watcher->addPath(m_directory); }
    QFileSystemWatcher* m_watcher;
    QString m_directory;
};
