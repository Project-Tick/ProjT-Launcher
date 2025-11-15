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
#include "minecraft/VersionFilterData.h"
#include "net/NetJob.h"
#include "tasks/Task.h"

class MinecraftInstance;

class FMLLibrariesTask : public Task {
    Q_OBJECT
   public:
    FMLLibrariesTask(MinecraftInstance* inst);
    virtual ~FMLLibrariesTask() = default;

    void executeTask() override;

    bool canAbort() const override;

   private slots:
    void fmllibsFinished();
    void fmllibsFailed(QString reason);

   public slots:
    bool abort() override;

   private:
    MinecraftInstance* m_inst;
    NetJob::Ptr downloadJob;
    QList<FMLlib> fmlLibsToProcess;
};
