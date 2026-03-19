// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2026 Project Tick
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
 *  along with this program.  If not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
/* === Upstream License Block (Do Not Modify) ==============================


======================================================================== */

#include "DataMigrationTask.h"

#include "FileSystem.h"

#include <QDirIterator>
#include <QFileInfo>
#include <QMap>

#include <QtConcurrent>

DataMigrationTask::DataMigrationTask(const QString& sourcePath, const QString& targetPath, Filter pathMatcher)
	: Task(),
	  m_sourcePath(sourcePath),
	  m_targetPath(targetPath),
	  m_pathMatcher(pathMatcher),
	  m_copy(sourcePath, targetPath)
{
	m_copy.matcher(m_pathMatcher).whitelist(true);
}

void DataMigrationTask::executeTask()
{
	setStatus(tr("Scanning files..."));

	// 1. Scan
	// Check how many files we gotta copy
	m_copyFuture = QtConcurrent::run(QThreadPool::globalInstance(),
									 [this]
									 {
										 return m_copy(true); // dry run to collect amount of files
									 });
	connect(&m_copyFutureWatcher, &QFutureWatcher<bool>::finished, this, &DataMigrationTask::dryRunFinished);
	connect(&m_copyFutureWatcher, &QFutureWatcher<bool>::canceled, this, &DataMigrationTask::dryRunAborted);
	m_copyFutureWatcher.setFuture(m_copyFuture);
}

void DataMigrationTask::dryRunFinished()
{
	disconnect(&m_copyFutureWatcher, &QFutureWatcher<bool>::finished, this, &DataMigrationTask::dryRunFinished);
	disconnect(&m_copyFutureWatcher, &QFutureWatcher<bool>::canceled, this, &DataMigrationTask::dryRunAborted);

	if (!m_copyFuture.isValid() || !m_copyFuture.result())
	{
		emitFailed(tr("Failed to scan source path."));
		return;
	}

	// 2. Copy
	// Actually copy all files now.
	m_toCopy = m_copy.totalCopied();
	connect(&m_copy,
			&FS::copy::fileCopied,
			[&, this](const QString& relativeName)
			{
				QString shortenedName = relativeName;
				// shorten the filename to hopefully fit into one line
				if (shortenedName.length() > 50)
					shortenedName = relativeName.left(20) + "…" + relativeName.right(29);
				setProgress(m_copy.totalCopied(), m_toCopy);
				setStatus(tr("Copying %1…").arg(shortenedName));
			});
	m_copyFuture = QtConcurrent::run(QThreadPool::globalInstance(),
									 [this]
									 {
										 return m_copy(false); // actually copy now
									 });
	connect(&m_copyFutureWatcher, &QFutureWatcher<bool>::finished, this, &DataMigrationTask::copyFinished);
	connect(&m_copyFutureWatcher, &QFutureWatcher<bool>::canceled, this, &DataMigrationTask::copyAborted);
	m_copyFutureWatcher.setFuture(m_copyFuture);
}

void DataMigrationTask::dryRunAborted()
{
	emitAborted();
}

void DataMigrationTask::copyFinished()
{
	disconnect(&m_copyFutureWatcher, &QFutureWatcher<bool>::finished, this, &DataMigrationTask::copyFinished);
	disconnect(&m_copyFutureWatcher, &QFutureWatcher<bool>::canceled, this, &DataMigrationTask::copyAborted);

	if (!m_copyFuture.isValid() || !m_copyFuture.result())
	{
		emitFailed(tr("Some paths could not be copied!"));
		return;
	}

	emitSucceeded();
}

void DataMigrationTask::copyAborted()
{
	emitAborted();
}
