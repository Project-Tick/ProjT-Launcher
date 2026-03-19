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

#include "minecraft/Component.h"
#include "net/Mode.h"
#include "tasks/Task.h"

#include <memory>
class PackProfile;
struct ComponentUpdateTaskData;

class ComponentUpdateTask : public Task
{
	Q_OBJECT
  public:
	enum class Mode
	{
		Launch,
		Resolution
	};

  public:
	explicit ComponentUpdateTask(Mode mode, Net::Mode netmode, PackProfile* list);
	virtual ~ComponentUpdateTask();

  protected:
	void executeTask() override;

  private:
	void loadComponents();
	/// collects components that are dependent on or dependencies of the component
	QList<ComponentPtr> collectTreeLinked(const QString& uid);
	void resolveDependencies(bool checkOnly);
	void performUpdateActions();
	void finalizeComponents();

	void remoteLoadSucceeded(size_t index);
	void remoteLoadFailed(size_t index, const QString& msg);
	void checkIfAllFinished();

  private:
	std::unique_ptr<ComponentUpdateTaskData> d;
};
