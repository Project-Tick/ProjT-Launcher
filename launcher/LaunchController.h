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
/* === Upstream License Block (Do Not Modify) ==============================
 *
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (C) 2022 Sefa Eyeoglu <contact@scrumplex.net>
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
 * This file incorporates work covered by the following copyright and
 * permission notice:
 *
 *      Copyright 2013-2021 MultiMC Contributors
 *
 *      Licensed under the Apache License, Version 2.0 (the "License");
 *      you may not use this file except in compliance with the License.
 *      You may obtain a copy of the License at
 *
 *          http://www.apache.org/licenses/LICENSE-2.0
 *
 *      Unless required by applicable law or agreed to in writing, software
 *      distributed under the License is distributed on an "AS IS" BASIS,
 *      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *      See the License for the specific language governing permissions and
 *      limitations under the License.
 *
 * ======================================================================== */

#pragma once

#include <BaseInstance.h>
#include <QObject>
#include <tools/BaseProfiler.h>

#include "LaunchMode.h"
#include "minecraft/auth/MinecraftAccount.hpp"
#include "minecraft/launch/MinecraftTarget.hpp"

class InstanceWindow;

namespace projt::launch
{
	class LaunchPipeline;
}

enum class LaunchDecision
{
	Undecided,
	Continue,
	Abort
};

class LaunchController : public Task
{
	Q_OBJECT

  public:
	void executeTask() override;

	LaunchController();
	~LaunchController() override = default;

	void setInstance(InstancePtr instance)
	{
		m_instance = instance;
	}

	InstancePtr instance()
	{
		return m_instance;
	}

	void setLaunchMode(LaunchMode mode)
	{
		m_wantedLaunchMode = mode;
	}

	void setOfflineName(const QString& offlineName)
	{
		m_offlineName = offlineName;
	}

	void setProfiler(BaseProfilerFactory* profiler)
	{
		m_profiler = profiler;
	}

	void setParentWidget(QWidget* widget)
	{
		m_parentWidget = widget;
	}

	void setTargetToJoin(MinecraftTarget::Ptr targetToJoin)
	{
		m_targetToJoin = std::move(targetToJoin);
	}

	void setAccountToUse(MinecraftAccountPtr accountToUse)
	{
		m_accountToUse = std::move(accountToUse);
	}

	QString id()
	{
		return m_instance->id();
	}

	bool abort() override;

  private:
	void login();
	void launchInstance();
	void decideAccount();
	LaunchDecision decideLaunchMode();
	bool askPlayDemo();
	QString askOfflineName(QString playerName, bool* ok = nullptr);
	bool reauthenticateAccount(MinecraftAccountPtr account, QString reason = {});

  private slots:
	void readyForLaunch();
	void onSucceeded();
	void onFailed(QString reason);
	void onProgressRequested(Task* task);

  private:
	LaunchMode m_wantedLaunchMode = LaunchMode::Normal;
	LaunchMode m_actualLaunchMode = LaunchMode::Normal;
	BaseProfilerFactory* m_profiler = nullptr;
	QString m_offlineName;
	InstancePtr m_instance;
	QWidget* m_parentWidget = nullptr;
	InstanceWindow* m_console = nullptr;
	MinecraftAccountPtr m_accountToUse = nullptr;
	AuthSessionPtr m_session;
	shared_qobject_ptr<projt::launch::LaunchPipeline> m_launcher;
	MinecraftTarget::Ptr m_targetToJoin;
};
