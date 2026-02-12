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

#include <QUrl>

#include "tasks/Task.h"

namespace projt::java
{
	class RuntimeManifestTask : public Task
	{
		Q_OBJECT
	  public:
		RuntimeManifestTask(QUrl url, QString finalPath, QString checksumType = "", QString checksumHash = "");
		~RuntimeManifestTask() override = default;

		bool canAbort() const override
		{
			return true;
		}

		bool abort() override;

	  protected:
		void executeTask() override;

	  private:
		void downloadRuntime(const QJsonDocument& doc);

		QUrl m_url;
		QString m_final_path;
		QString m_checksum_type;
		QString m_checksum_hash;
		Task::Ptr m_task;
	};
} // namespace projt::java