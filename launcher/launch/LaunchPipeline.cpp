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

#include "LaunchPipeline.hpp"

#include <utility>

#include "BaseInstance.h"

namespace projt::launch
{
	LaunchPipeline::Ptr LaunchPipeline::create(MinecraftInstancePtr instance)
	{
		return LaunchPipeline::Ptr(new LaunchPipeline(std::move(instance)));
	}

	LaunchPipeline::LaunchPipeline(MinecraftInstancePtr instance) : m_instanceRef(std::move(instance))
	{
		if (m_instanceRef)
		{
			m_instanceRef->setRunning(true);
		}
	}

	void LaunchPipeline::appendStage(StagePtr stage)
	{
		m_stageQueue.append(stage);
	}

	void LaunchPipeline::prependStage(StagePtr stage)
	{
		m_stageQueue.prepend(stage);
	}

	void LaunchPipeline::executeTask()
	{
		if (!m_instanceRef)
		{
			emitFailed(tr("Missing instance for launch."));
			return;
		}
		m_instanceRef->setCrashed(false);
		if (m_stageQueue.isEmpty())
		{
			m_state = State::Finished;
			emitSucceeded();
			return;
		}
		m_state = State::Running;
		advanceStage();
	}

	void LaunchPipeline::advanceStage()
	{
		if (m_activeStage && !m_activeStage->wasSuccessful())
		{
			closeStages(false, m_activeStage->failReason());
			return;
		}

		if (m_stageQueue.isEmpty())
		{
			closeStages(true, QString());
			return;
		}

		m_activeStage = m_stageQueue.takeFirst();
		m_stageHistory.append(m_activeStage);
		m_activeStage->start();
	}

	void LaunchPipeline::onReadyForLaunch()
	{
		m_state = State::Waiting;
		emit readyForLaunch();
	}

	void LaunchPipeline::onStageFinished()
	{
		advanceStage();
	}

	void LaunchPipeline::closeStages(bool successful, const QString& error)
	{
		for (int idx = m_stageHistory.size() - 1; idx >= 0; --idx)
		{
			m_stageHistory[idx]->finalize();
		}
		m_activeStage.reset();
		if (successful)
		{
			m_state = State::Finished;
			emitSucceeded();
		}
		else
		{
			m_state = State::Failed;
			emitFailed(error);
		}
	}

	void LaunchPipeline::onProgressReportingRequested()
	{
		if (!m_activeStage)
		{
			return;
		}
		m_state = State::Waiting;
		emit requestProgress(m_activeStage.get());
	}

	void LaunchPipeline::setCensorFilter(QMap<QString, QString> filter)
	{
		m_redactions = std::move(filter);
	}

	QString LaunchPipeline::censorPrivateInfo(QString input) const
	{
		for (auto it = m_redactions.begin(); it != m_redactions.end(); ++it)
		{
			input.replace(it.key(), it.value());
		}
		return input;
	}

	void LaunchPipeline::proceed()
	{
		if (m_state != State::Waiting)
		{
			return;
		}
		if (!m_activeStage)
		{
			return;
		}
		m_activeStage->proceed();
	}

	bool LaunchPipeline::canAbort() const
	{
		switch (m_state)
		{
			case State::Aborted:
			case State::Failed:
			case State::Finished: return false;
			case State::Idle: return true;
			case State::Running:
			case State::Waiting:
			{
				if (!m_activeStage)
				{
					return false;
				}
				return m_activeStage->canAbort();
			}
		}
		return false;
	}

	bool LaunchPipeline::abort()
	{
		switch (m_state)
		{
			case State::Aborted:
			case State::Failed:
			case State::Finished: return true;
			case State::Idle:
			{
				m_state = State::Aborted;
				emitAborted();
				return true;
			}
			case State::Running:
			case State::Waiting:
			{
				if (!m_activeStage)
				{
					return false;
				}
				if (!m_activeStage->canAbort())
				{
					return false;
				}
				if (m_activeStage->abort())
				{
					m_state = State::Aborted;
					return true;
				}
				break;
			}
		}
		return false;
	}

	shared_qobject_ptr<LaunchLogModel> LaunchPipeline::logModel()
	{
		if (!m_logStore)
		{
			m_logStore.reset(new LaunchLogModel());
			m_logStore->setMaxLines(getConsoleMaxLines(m_instanceRef->settings()));
			m_logStore->setStopOnOverflow(shouldStopOnConsoleOverflow(m_instanceRef->settings()));
			m_logStore->setOverflowMessage(
				tr("Stopped watching the game log because the log length surpassed %1 lines.\n"
				   "You may have to fix your mods because the game is still logging to files and"
				   " likely wasting harddrive space at an alarming rate!")
					.arg(m_logStore->getMaxLines()));
		}
		return m_logStore;
	}

	void LaunchPipeline::onLogLines(const QStringList& lines, MessageLevel::Enum defaultLevel)
	{
		for (const auto& line : lines)
		{
			onLogLine(line, defaultLevel);
		}
	}

	void LaunchPipeline::onLogLine(QString line, MessageLevel::Enum level)
	{
		m_lineRouter.routeLine(std::move(line), level, *logModel(), m_redactions);
	}

	void LaunchPipeline::emitSucceeded()
	{
		if (m_instanceRef)
		{
			m_instanceRef->setRunning(false);
		}
		Task::emitSucceeded();
	}

	void LaunchPipeline::emitFailed(QString reason)
	{
		if (m_instanceRef)
		{
			m_instanceRef->setRunning(false);
			m_instanceRef->setCrashed(true);
		}
		Task::emitFailed(reason);
	}

	QString LaunchPipeline::substituteVariables(const QString& cmd, bool isLaunch) const
	{
		if (!m_instanceRef)
		{
			return cmd;
		}
		auto env = isLaunch ? m_instanceRef->createLaunchEnvironment() : m_instanceRef->createEnvironment();
		return m_expander.expand(cmd, env);
	}
} // namespace projt::launch
