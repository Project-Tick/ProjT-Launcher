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

#include <QList>
#include <QString>
#include <cstddef>
#include "net/Mode.h"
#include "tasks/Task.h"

#include "minecraft/ComponentUpdateTask.h"

class PackProfile;

struct RemoteLoadStatus {
    enum class Type { Index, List, Version } type = Type::Version;
    size_t PackProfileIndex = 0;
    bool finished = false;
    bool succeeded = false;
    Task::Ptr task;
};

struct ComponentUpdateTaskData {
    PackProfile* m_profile = nullptr;
    QList<RemoteLoadStatus> remoteLoadStatusList;
    bool remoteLoadSuccessful = true;
    size_t remoteTasksInProgress = 0;
    ComponentUpdateTask::Mode mode;
    Net::Mode netmode;
};
