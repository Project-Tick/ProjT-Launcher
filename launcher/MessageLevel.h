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

#include <qlogging.h>
#include <QString>

/**
 * @brief the MessageLevel Enum
 * defines what level a log message is
 */
namespace MessageLevel
{
	enum Enum
	{
		Unknown,  /**< No idea what this is or where it came from */
		StdOut,	  /**< Undetermined stderr messages */
		StdErr,	  /**< Undetermined stdout messages */
		Launcher, /**< Launcher Messages */
		Trace,	  /**< Trace Messages */
		Debug,	  /**< Debug Messages */
		Info,	  /**< Info Messages */
		Message,  /**< Standard Messages */
		Warning,  /**< Warnings */
		Error,	  /**< Errors */
		Fatal,	  /**< Fatal Errors */
	};
	MessageLevel::Enum getLevel(const QString& levelName);
	MessageLevel::Enum getLevel(QtMsgType type);

	/* Get message level from a line. Line is modified if it was successful. */
	MessageLevel::Enum fromLine(QString& line);

	/* Get message level from a line from the launcher log. Line is modified if it was successful. */
	MessageLevel::Enum fromLauncherLine(QString& line);
} // namespace MessageLevel
