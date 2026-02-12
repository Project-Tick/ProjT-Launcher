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
#include "net/NetJob.h"
#include "tasks/Task.h"
class MinecraftInstance;

class AssetUpdateTask : public Task
{
	Q_OBJECT
  public:
	AssetUpdateTask(MinecraftInstance* inst);
	virtual ~AssetUpdateTask() = default;

	void executeTask() override;

	bool canAbort() const override;

  private slots:
	void assetIndexFinished();
	void assetIndexFailed(QString reason);
	void assetsFailed(QString reason);

  public slots:
	bool abort() override;

  private:
	MinecraftInstance* m_inst;
	NetJob::Ptr downloadJob;
};
