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

#include <LoggedProcess.h>
#include <launch/LaunchStage.hpp>

#include <QString>

namespace projt::launch::steps
{
	class LaunchCommandStep : public projt::launch::LaunchStage
	{
		Q_OBJECT
	  public:
		enum class Hook
		{
			PreLaunch,
			PostExit
		};

		LaunchCommandStep(projt::launch::LaunchPipeline* parent, Hook hook, QString commandTemplate);
		~LaunchCommandStep() override = default;

		void executeTask() override;
		bool canAbort() const override
		{
			return true;
		}
		bool abort() override;

		void setWorkingDirectory(const QString& workDir);

	  private slots:
		void onProcessState(LoggedProcess::State state);

	  private:
		QString hookLabel() const;
		QString buildFailure() const;

		LoggedProcess m_runner;
		QString m_commandTemplate;
		Hook m_hook;
	};
} // namespace projt::launch::steps
