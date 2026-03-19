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
#include "MinecraftLoadAndCheck.h"
#include "MinecraftInstance.h"
#include "PackProfile.h"

MinecraftLoadAndCheck::MinecraftLoadAndCheck(MinecraftInstance* inst, Net::Mode netmode)
	: m_inst(inst),
	  m_netmode(netmode)
{}

void MinecraftLoadAndCheck::executeTask()
{
	// add offline metadata load task
	auto components = m_inst->getPackProfile();
	if (auto result = components->reload(m_netmode); !result)
	{
		emitFailed(result.error);
		return;
	}
	m_task = components->getCurrentTask();

	if (!m_task)
	{
		emitSucceeded();
		return;
	}
	connect(m_task.get(), &Task::succeeded, this, &MinecraftLoadAndCheck::emitSucceeded);
	connect(m_task.get(), &Task::failed, this, &MinecraftLoadAndCheck::emitFailed);
	connect(m_task.get(), &Task::aborted, this, &MinecraftLoadAndCheck::emitAborted);
	connect(m_task.get(), &Task::progress, this, &MinecraftLoadAndCheck::setProgress);
	connect(m_task.get(), &Task::stepProgress, this, &MinecraftLoadAndCheck::propagateStepProgress);
	connect(m_task.get(), &Task::status, this, &MinecraftLoadAndCheck::setStatus);
	connect(m_task.get(), &Task::details, this, &MinecraftLoadAndCheck::setDetails);
}

bool MinecraftLoadAndCheck::canAbort() const
{
	if (m_task)
	{
		return m_task->canAbort();
	}
	return true;
}

bool MinecraftLoadAndCheck::abort()
{
	if (m_task && m_task->canAbort())
	{
		return m_task->abort();
	}
	return Task::abort();
}
