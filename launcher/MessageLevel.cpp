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
#include "MessageLevel.h"

MessageLevel::Enum MessageLevel::getLevel(const QString& levelName)
{
	QString name = levelName.toUpper();
	if (name == "LAUNCHER")
		return MessageLevel::Launcher;
	else if (name == "TRACE")
		return MessageLevel::Trace;
	else if (name == "DEBUG")
		return MessageLevel::Debug;
	else if (name == "INFO")
		return MessageLevel::Info;
	else if (name == "MESSAGE")
		return MessageLevel::Message;
	else if (name == "WARNING" || name == "WARN")
		return MessageLevel::Warning;
	else if (name == "ERROR" || name == "CRITICAL")
		return MessageLevel::Error;
	else if (name == "FATAL")
		return MessageLevel::Fatal;
	// Skip PrePost, it's not exposed to !![]!
	// Also skip StdErr and StdOut
	else
		return MessageLevel::Unknown;
}

MessageLevel::Enum MessageLevel::getLevel(QtMsgType type)
{
	switch (type)
	{
		case QtDebugMsg: return MessageLevel::Debug;
		case QtInfoMsg: return MessageLevel::Info;
		case QtWarningMsg: return MessageLevel::Warning;
		case QtCriticalMsg: return MessageLevel::Error;
		case QtFatalMsg: return MessageLevel::Fatal;
		default: return MessageLevel::Unknown;
	}
}

MessageLevel::Enum MessageLevel::fromLine(QString& line)
{
	// Level prefix
	int endmark = line.indexOf("]!");
	if (line.startsWith("!![") && endmark != -1)
	{
		auto level = MessageLevel::getLevel(line.left(endmark).mid(3));
		line	   = line.mid(endmark + 2);
		return level;
	}
	return MessageLevel::Unknown;
}

MessageLevel::Enum MessageLevel::fromLauncherLine(QString& line)
{
	// Level prefix
	int startMark = 0;
	while (startMark < line.size()
		   && (line[startMark].isDigit() || line[startMark].isSpace() || line[startMark] == '.'))
		++startMark;
	int endmark = line.indexOf(":");
	if (startMark < line.size() && endmark != -1)
	{
		auto level = MessageLevel::getLevel(line.left(endmark).mid(startMark));
		line	   = line.mid(endmark + 2);
		return level;
	}
	return MessageLevel::Unknown;
}
