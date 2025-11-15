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

#pragma once

#include "FileSystem.h"
#include "Filter.h"
#include "tasks/Task.h"

#include <QFuture>
#include <QFutureWatcher>

/*
 * Migrate existing data from other MMC-like launchers.
 */

class DataMigrationTask : public Task {
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
