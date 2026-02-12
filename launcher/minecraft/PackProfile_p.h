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
#pragma once

#include <QList>
#include <QMap>
#include <QTimer>
#include "Component.h"
#include "tasks/Task.h"

class MinecraftInstance;
using ComponentContainer = QList<ComponentPtr>;
using ComponentIndex	 = QMap<QString, ComponentPtr>;

struct PackProfileData
{
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
	bool loaded				 = false;
	bool interactionDisabled = true;
};
