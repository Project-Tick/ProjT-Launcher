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

#pragma once

#include "FileSystem.h"
#include "Filter.h"
#include "tasks/Task.h"

#include <QFuture>
#include <QFutureWatcher>

/*
 * Migrate existing data from other MMC-like launchers.
 */

class DataMigrationTask : public Task
{
	Q_OBJECT
  public:
	explicit DataMigrationTask(const QString& sourcePath, const QString& targetPath, Filter pathmatcher);
	~DataMigrationTask() override = default;

  protected:
	virtual void executeTask() override;

  protected slots:
	void dryRunFinished();
	void dryRunAborted();
	void copyFinished();
	void copyAborted();

  private:
	const QString& m_sourcePath;
	const QString& m_targetPath;
	const Filter m_pathMatcher;

	FS::copy m_copy;
	int m_toCopy = 0;
	QFuture<bool> m_copyFuture;
	QFutureWatcher<bool> m_copyFutureWatcher;
};
