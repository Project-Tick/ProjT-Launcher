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

#include <launch/LaunchStage.hpp>

#include <QHash>
#include <QStringList>

class QHostInfo;

namespace projt::launch::steps
{
	class HostLookupReportStep : public projt::launch::LaunchStage
	{
		Q_OBJECT
	  public:
		HostLookupReportStep(projt::launch::LaunchPipeline* parent, QStringList hosts);
		~HostLookupReportStep() override = default;

		void executeTask() override;
		bool canAbort() const override;

	  private slots:
		void onLookupFinished(const QHostInfo& info);

	  private:
		void finalizeIfReady();
		QString formatMessage(const QString& host, const QHostInfo& info) const;

		QHash<int, QString> m_lookupById;
		QHash<QString, QString> m_messages;
		QStringList m_hosts;
		int m_pending = 0;
	};
} // namespace projt::launch::steps
