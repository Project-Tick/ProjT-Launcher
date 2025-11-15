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
#include "BaseProfiler.h"
#include "QObjectPtr.h"

#include <QProcess>

BaseProfiler::BaseProfiler(SettingsObjectPtr settings, InstancePtr instance, QObject* parent) : BaseExternalTool(settings, instance, parent)
{}

void BaseProfiler::beginProfiling(shared_qobject_ptr<LaunchTask> process)
{
    beginProfilingImpl(process);
}

void BaseProfiler::abortProfiling()
{
    abortProfilingImpl();
}

void BaseProfiler::abortProfilingImpl()
{
    if (!m_profilerProcess) {
        return;
    }
    m_profilerProcess->terminate();
    m_profilerProcess->deleteLater();
    m_profilerProcess = 0;
    emit abortLaunch(tr("Profiler aborted"));
}

BaseProfiler* BaseProfilerFactory::createProfiler(InstancePtr instance, QObject* parent)
{
    return qobject_cast<BaseProfiler*>(createTool(instance, parent));
}
