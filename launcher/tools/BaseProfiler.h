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

#include "BaseExternalTool.h"
#include "QObjectPtr.h"

class BaseInstance;
class SettingsObject;
namespace projt::launch
{
	class LaunchPipeline;
}
class QProcess;

class BaseProfiler : public BaseExternalTool
{
	Q_OBJECT
  public:
	explicit BaseProfiler(SettingsObjectPtr settings, InstancePtr instance, QObject* parent = 0);

  public slots:
	void beginProfiling(shared_qobject_ptr<projt::launch::LaunchPipeline> process);
	void abortProfiling();

  protected:
	QProcess* m_profilerProcess;

	virtual void beginProfilingImpl(shared_qobject_ptr<projt::launch::LaunchPipeline> process) = 0;
	virtual void abortProfilingImpl();

  signals:
	void readyToLaunch(const QString& message);
	void abortLaunch(const QString& message);
};

class BaseProfilerFactory : public BaseExternalToolFactory
{
  public:
	virtual BaseProfiler* createProfiler(InstancePtr instance, QObject* parent = 0);
};
