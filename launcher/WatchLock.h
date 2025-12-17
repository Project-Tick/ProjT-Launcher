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
