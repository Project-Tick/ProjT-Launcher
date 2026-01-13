// SPDX-License-Identifier: GPL-3.0-only AND Apache-2.0
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
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * === Upstream License Block (Do Not Modify) ==============================
 *
 * Copyright 2013-2021 MultiMC Contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ======================================================================== */

#include "TaskStepWrapper.h"
#include "tasks/Task.h"

void TaskStepWrapper::executeTask()
{
	if (m_state == Task::State::AbortedByUser)
	{
		emitFailed(tr("Task aborted."));
		return;
	}
	connect(m_task.get(), &Task::finished, this, &TaskStepWrapper::updateFinished);
	connect(m_task.get(), &Task::progress, this, &TaskStepWrapper::setProgress);
	connect(m_task.get(), &Task::stepProgress, this, &TaskStepWrapper::propagateStepProgress);
	connect(m_task.get(), &Task::status, this, &TaskStepWrapper::setStatus);
	connect(m_task.get(), &Task::details, this, &TaskStepWrapper::setDetails);
	emit progressReportingRequest();
}

void TaskStepWrapper::proceed()
{
	m_task->start();
}

void TaskStepWrapper::updateFinished()
{
	if (m_task->wasSuccessful())
	{
		m_task.reset();
		emitSucceeded();
	}
	else
	{
		QString reason = tr("Instance update failed because: %1\n\n").arg(m_task->failReason());
		m_task.reset();
		emit logLine(reason, MessageLevel::Fatal);
		emitFailed(reason);
	}
}

bool TaskStepWrapper::canAbort() const
{
	if (m_task)
	{
		return m_task->canAbort();
	}
	return true;
}

bool TaskStepWrapper::abort()
{
	if (m_task && m_task->canAbort())
	{
		auto status = m_task->abort();
		emitFailed("Aborted.");
		return status;
	}
	return Task::abort();
}
