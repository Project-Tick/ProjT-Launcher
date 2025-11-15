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
 *
 * === Upstream License Block (Do Not Modify) ==============================
 *
 * // SPDX-License-Identifier: GPL-3.0-only
 *
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (c) 2023 Trial97 <alexandru.tripon97@gmail.com>
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
 *
 *
 *
 *
 * ======================================================================== */
#include "GenericProfiler.h"

#include "BaseInstance.h"
#include "launch/LaunchTask.h"
#include "settings/SettingsObject.h"

class GenericProfiler : public BaseProfiler {
    Q_OBJECT
   public:
    GenericProfiler(SettingsObjectPtr settings, InstancePtr instance, QObject* parent = 0);

   protected:
    void beginProfilingImpl(shared_qobject_ptr<LaunchTask> process);
};

GenericProfiler::GenericProfiler(SettingsObjectPtr settings, InstancePtr instance, QObject* parent)
    : BaseProfiler(settings, instance, parent)
{}

void GenericProfiler::beginProfilingImpl(shared_qobject_ptr<LaunchTask> process)
{
    emit readyToLaunch(tr("Started process: %1").arg(process->pid()));
}

BaseExternalTool* GenericProfilerFactory::createTool(InstancePtr instance, QObject* parent)
{
    return new GenericProfiler(globalSettings, instance, parent);
}
#include "GenericProfiler.moc"