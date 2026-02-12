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
 *  along with this program.  If not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
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

#pragma once

#include <LoggedProcess.h>
#include <launch/LaunchStage.hpp>
#include <minecraft/auth/AuthSession.hpp>

#include "MinecraftTarget.hpp"

class LauncherPartLaunch : public projt::launch::LaunchStage
{
	Q_OBJECT
  public:
	explicit LauncherPartLaunch(projt::launch::LaunchPipeline* parent);
	virtual ~LauncherPartLaunch() = default;

	virtual void executeTask();
	virtual bool abort();
	virtual void proceed();
	virtual bool canAbort() const
	{
		return true;
	}
	void setWorkingDirectory(const QString& wd);
	void setAuthSession(AuthSessionPtr session)
	{
		m_session = session;
	}

	void setTargetToJoin(MinecraftTarget::Ptr targetToJoin)
	{
		m_targetToJoin = std::move(targetToJoin);
	}

  private slots:
	void on_state(LoggedProcess::State state);

  private:
	LoggedProcess m_process;
	QString m_command;
	AuthSessionPtr m_session;
	QString m_launchScript;
	MinecraftTarget::Ptr m_targetToJoin;

	bool mayProceed = false;
};
