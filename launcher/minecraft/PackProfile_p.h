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
#include <QMap>
#include <QTimer>
#include "Component.h"
#include "tasks/Task.h"

class MinecraftInstance;
using ComponentContainer = QList<ComponentPtr>;
using ComponentIndex = QMap<QString, ComponentPtr>;

struct PackProfileData {
    // the instance this belongs to
    MinecraftInstance* m_instance;

    // the launch profile (volatile, temporary thing created on demand)
    std::shared_ptr<LaunchProfile> m_profile;

    // persistent list of components and related machinery
    ComponentContainer components;
    ComponentIndex componentIndex;
    bool dirty = false;
    QTimer m_saveTimer;
    Task::Ptr m_updateTask;
    bool loaded = false;
    bool interactionDisabled = true;
};
