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

#include <QDir>
#include <QFileSystemWatcher>
#include "Filter.h"

class RecursiveFileSystemWatcher : public QObject {
    Q_OBJECT
   public:
    RecursiveFileSystemWatcher(QObject* parent);

    void setRootDir(const QDir& root);
    QDir rootDir() const { return m_root; }

    // WARNING: setting this to true may be bad for performance
    void setWatchFiles(bool watchFiles);
    bool watchFiles() const { return m_watchFiles; }

    void setMatcher(Filter matcher) { m_matcher = std::move(matcher); }

    QStringList files() const { return m_files; }

   signals:
    void filesChanged();
    void fileChanged(const QString& path);

   public slots:
    void enable();
    void disable();

   private:
    QDir m_root;
    bool m_watchFiles = false;
    bool m_isEnabled = false;
    Filter m_matcher;

    QFileSystemWatcher* m_watcher;

    QStringList m_files;
    void setFiles(const QStringList& files);

    void addFilesToWatcherRecursive(const QDir& dir);
    QStringList scanRecursive(const QDir& dir);

   private slots:
    void fileChange(const QString& path);
    void directoryChange(const QString& path);
};
