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

#include "BaseExternalTool.h"
#include "QObjectPtr.h"

class BaseInstance;
class SettingsObject;
class LaunchTask;
class QProcess;

class BaseProfiler : public BaseExternalTool {
    Q_OBJECT
   public:
    explicit BaseProfiler(SettingsObjectPtr settings, InstancePtr instance, QObject* parent = 0);

   public slots:
    void beginProfiling(shared_qobject_ptr<LaunchTask> process);
    void abortProfiling();

   protected:
    QProcess* m_profilerProcess;

    virtual void beginProfilingImpl(shared_qobject_ptr<LaunchTask> process) = 0;
    virtual void abortProfilingImpl();

   signals:
    void readyToLaunch(const QString& message);
    void abortLaunch(const QString& message);
};

class BaseProfilerFactory : public BaseExternalToolFactory {
   public:
    virtual BaseProfiler* createProfiler(InstancePtr instance, QObject* parent = 0);
};
