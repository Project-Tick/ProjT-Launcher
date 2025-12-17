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

#include <QFuture>
#include <QFutureWatcher>
#include <QUrl>
#include "BaseInstance.h"
#include "BaseVersion.h"
#include "Filter.h"
#include "InstanceCopyPrefs.h"
#include "InstanceTask.h"
#include "net/NetJob.h"
#include "settings/SettingsObject.h"
#include "tasks/Task.h"

class InstanceCopyTask : public InstanceTask {
    Q_OBJECT
   public:
    explicit InstanceCopyTask(InstancePtr origInstance, const InstanceCopyPrefs& prefs);

   protected:
    //! Entry point for tasks.
    virtual void executeTask() override;
    bool abort() override;
    void copyFinished();
    void copyAborted();

   private:
    /* data */
    InstancePtr m_origInstance;
    QFuture<bool> m_copyFuture;
    QFutureWatcher<bool> m_copyFutureWatcher;
    Filter m_matcher;
    bool m_keepPlaytime;
    bool m_useLinks = false;
    bool m_useHardLinks = false;
    bool m_copySaves = false;
    bool m_linkRecursively = false;
    bool m_useClone = false;
};
