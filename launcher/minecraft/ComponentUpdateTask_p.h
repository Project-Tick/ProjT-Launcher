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
#include <QString>
#include <cstddef>
#include "net/Mode.h"
#include "tasks/Task.h"

#include "minecraft/ComponentUpdateTask.h"

class PackProfile;

struct RemoteLoadStatus
{
	enum class Type
	{
		Index,
		List,
		Version
	} type					= Type::Version;
	size_t PackProfileIndex = 0;
	bool finished			= false;
	bool succeeded			= false;
	Task::Ptr task;
};

struct ComponentUpdateTaskData
{
	PackProfile* m_profile = nullptr;
	QList<RemoteLoadStatus> remoteLoadStatusList;
	bool remoteLoadSuccessful	 = true;
	size_t remoteTasksInProgress = 0;
	ComponentUpdateTask::Mode mode;
	Net::Mode netmode;
};
