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

#include "TaskBridgeStage.hpp"

#include "tasks/Task.h"

namespace projt::launch
{
	TaskBridgeStage::TaskBridgeStage(LaunchPipeline* pipeline, Task::Ptr task)
		: LaunchStage(pipeline),
		  m_payload(std::move(task))
	{}

	void TaskBridgeStage::executeTask()
	{
		if (m_state == Task::State::AbortedByUser)
		{
			emitFailed(tr("Task aborted."));
			return;
		}
		connect(m_payload.get(), &Task::finished, this, &TaskBridgeStage::onPayloadFinished);
		connect(m_payload.get(), &Task::progress, this, &TaskBridgeStage::setProgress);
		connect(m_payload.get(), &Task::stepProgress, this, &TaskBridgeStage::propagateStepProgress);
		connect(m_payload.get(), &Task::status, this, &TaskBridgeStage::setStatus);
		connect(m_payload.get(), &Task::details, this, &TaskBridgeStage::setDetails);
		emit progressReportingRequest();
	}

	void TaskBridgeStage::proceed()
	{
		if (m_payload)
		{
			m_payload->start();
		}
	}

	void TaskBridgeStage::onPayloadFinished()
	{
		if (!m_payload)
		{
			emitFailed(tr("Task is missing."));
			return;
		}
		if (m_payload->wasSuccessful())
		{
			m_payload.reset();
			emitSucceeded();
		}
		else
		{
			QString reason = tr("Instance update failed because: %1\n\n").arg(m_payload->failReason());
			m_payload.reset();
			emit logLine(reason, MessageLevel::Fatal);
			emitFailed(reason);
		}
	}

	bool TaskBridgeStage::canAbort() const
	{
		if (m_payload)
		{
			return m_payload->canAbort();
		}
		return true;
	}

	bool TaskBridgeStage::abort()
	{
		if (m_payload && m_payload->canAbort())
		{
			auto status = m_payload->abort();
			emitFailed("Aborted.");
			return status;
		}
		return Task::abort();
	}
} // namespace projt::launch
