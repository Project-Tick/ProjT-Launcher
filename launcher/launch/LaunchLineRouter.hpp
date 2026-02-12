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

#include <QMap>
#include <QString>

#include "MessageLevel.h"
#include "logs/LogEventParser.hpp"

namespace projt::launch
{
	class LaunchLogModel;

	class LaunchLineRouter
	{
	  public:
		void routeLine(QString line,
					   MessageLevel::Enum defaultLevel,
					   LaunchLogModel& model,
					   const QMap<QString, QString>& redactions);

	  private:
		bool parseStructured(const QString& line,
							 MessageLevel::Enum level,
							 LaunchLogModel& model,
							 const QMap<QString, QString>& redactions);
		QString applyRedactions(QString input, const QMap<QString, QString>& redactions) const;

		projt::logs::LogEventParser m_stdoutParser;
		projt::logs::LogEventParser m_stderrParser;
	};
} // namespace projt::launch
