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
/* === Upstream License Block (Do Not Modify) ==============================

// SPDX-FileCopyrightText: 2022 Sefa Eyeoglu <contact@scrumplex.net>
//
// SPDX-License-Identifier: GPL-3.0-only

======================================================================== */

#include "DataMigrationTask.h"

#include "FileSystem.h"

#include <QDirIterator>
#include <QFileInfo>
#include <QMap>

#include <QtConcurrent>

DataMigrationTask::DataMigrationTask(const QString& sourcePath, const QString& targetPath, Filter pathMatcher)
    : Task(), m_sourcePath(sourcePath), m_targetPath(targetPath), m_pathMatcher(pathMatcher), m_copy(sourcePath, targetPath)
{
    m_copy.matcher(m_pathMatcher).whitelist(true);
}

void DataMigrationTask::executeTask()
{
    setStatus(tr("Scanning files..."));

    // 1. Scan
    // Check how many files we gotta copy
    m_copyFuture = QtConcurrent::run(QThreadPool::globalInstance(), [this] {
        return m_copy(true);  // dry run to collect amount of files
    });
    connect(&m_copyFutureWatcher, &QFutureWatcher<bool>::finished, this, &DataMigrationTask::dryRunFinished);
    connect(&m_copyFutureWatcher, &QFutureWatcher<bool>::canceled, this, &DataMigrationTask::dryRunAborted);
    m_copyFutureWatcher.setFuture(m_copyFuture);
}

void DataMigrationTask::dryRunFinished()
{
    disconnect(&m_copyFutureWatcher, &QFutureWatcher<bool>::finished, this, &DataMigrationTask::dryRunFinished);
    disconnect(&m_copyFutureWatcher, &QFutureWatcher<bool>::canceled, this, &DataMigrationTask::dryRunAborted);

    if (!m_copyFuture.isValid() || !m_copyFuture.result()) {
        emitFailed(tr("Failed to scan source path."));
        return;
    }

    // 2. Copy
    // Actually copy all files now.
    m_toCopy = m_copy.totalCopied();
    connect(&m_copy, &FS::copy::fileCopied, [&, this](const QString& relativeName) {
        QString shortenedName = relativeName;
        // shorten the filename to hopefully fit into one line
        if (shortenedName.length() > 50)
            shortenedName = relativeName.left(20) + "…" + relativeName.right(29);
        setProgress(m_copy.totalCopied(), m_toCopy);
        setStatus(tr("Copying %1…").arg(shortenedName));
    });
    m_copyFuture = QtConcurrent::run(QThreadPool::globalInstance(), [this] {
        return m_copy(false);  // actually copy now
    });
    connect(&m_copyFutureWatcher, &QFutureWatcher<bool>::finished, this, &DataMigrationTask::copyFinished);
    connect(&m_copyFutureWatcher, &QFutureWatcher<bool>::canceled, this, &DataMigrationTask::copyAborted);
    m_copyFutureWatcher.setFuture(m_copyFuture);
}

void DataMigrationTask::dryRunAborted()
{
    emitFailed(tr("Aborted"));
}

void DataMigrationTask::copyFinished()
{
    disconnect(&m_copyFutureWatcher, &QFutureWatcher<bool>::finished, this, &DataMigrationTask::copyFinished);
    disconnect(&m_copyFutureWatcher, &QFutureWatcher<bool>::canceled, this, &DataMigrationTask::copyAborted);

    if (!m_copyFuture.isValid() || !m_copyFuture.result()) {
        emitFailed(tr("Some paths could not be copied!"));
        return;
    }

    emitSucceeded();
}

void DataMigrationTask::copyAborted()
{
    emitFailed(tr("Aborted"));
}
