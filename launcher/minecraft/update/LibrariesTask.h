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
#pragma once
#include "net/NetJob.h"
#include "tasks/Task.h"
class MinecraftInstance;

class LibrariesTask : public Task {
    Q_OBJECT
   public:
    LibrariesTask(MinecraftInstance* inst);
    virtual ~LibrariesTask() = default;

    void executeTask() override;

    bool canAbort() const override;

   private slots:
    void jarlibFailed(QString reason);

   public slots:
    bool abort() override;

   private:
    MinecraftInstance* m_inst;
    NetJob::Ptr downloadJob;
};
