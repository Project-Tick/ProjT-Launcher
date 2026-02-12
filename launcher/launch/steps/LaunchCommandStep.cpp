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

#include "LaunchCommandStep.hpp"

#include <launch/LaunchPipeline.hpp>

#include <QProcess>

namespace projt::launch::steps
{
	LaunchCommandStep::LaunchCommandStep(projt::launch::LaunchPipeline* parent, Hook hook, QString commandTemplate)
		: projt::launch::LaunchStage(parent),
		  m_commandTemplate(commandTemplate),
		  m_hook(hook)
	{
		auto instance = m_flow->instance();
		m_runner.setProcessEnvironment(instance->createEnvironment());
		connect(&m_runner, &LoggedProcess::log, this, &LaunchCommandStep::logLines);
		connect(&m_runner, &LoggedProcess::stateChanged, this, &LaunchCommandStep::onProcessState);
	}

	void LaunchCommandStep::executeTask()
	{
		QString commandLine = m_flow->substituteVariables(m_commandTemplate).trimmed();
		if (commandLine.isEmpty())
		{
			auto error = tr("%1 command is empty.").arg(hookLabel());
			emit logLine(error, MessageLevel::Error);
			emitFailed(error);
			return;
		}

		emit logLine(tr("Running %1 command: %2").arg(hookLabel(), commandLine), MessageLevel::Launcher);

		auto parts = QProcess::splitCommand(commandLine);
		if (parts.isEmpty())
		{
			auto error = tr("%1 command is empty.").arg(hookLabel());
			emit logLine(error, MessageLevel::Error);
			emitFailed(error);
			return;
		}

		QString program = parts.takeFirst();
		m_runner.start(program, parts);
	}

	void LaunchCommandStep::onProcessState(LoggedProcess::State state)
	{
		switch (state)
		{
			case LoggedProcess::Aborted:
			case LoggedProcess::Crashed:
			case LoggedProcess::FailedToStart:
			{
				auto error = buildFailure();
				emit logLine(error, MessageLevel::Fatal);
				emitFailed(error);
				return;
			}
			case LoggedProcess::Finished:
			{
				if (m_runner.exitCode() != 0)
				{
					auto error = buildFailure();
					emit logLine(error, MessageLevel::Fatal);
					emitFailed(error);
				}
				else
				{
					emit logLine(tr("%1 command ran successfully.\n\n").arg(hookLabel()), MessageLevel::Launcher);
					emitSucceeded();
				}
				return;
			}
			default: break;
		}
	}

	void LaunchCommandStep::setWorkingDirectory(const QString& workDir)
	{
		m_runner.setWorkingDirectory(workDir);
	}

	bool LaunchCommandStep::abort()
	{
		auto state = m_runner.state();
		if (state == LoggedProcess::Running || state == LoggedProcess::Starting)
		{
			m_runner.kill();
		}
		return true;
	}

	QString LaunchCommandStep::hookLabel() const
	{
		switch (m_hook)
		{
			case Hook::PreLaunch: return tr("Pre-Launch");
			case Hook::PostExit: return tr("Post-Launch");
		}
		return tr("Launch");
	}

	QString LaunchCommandStep::buildFailure() const
	{
		return tr("%1 command failed with code %2.\n\n").arg(hookLabel()).arg(m_runner.exitCode());
	}
} // namespace projt::launch::steps
