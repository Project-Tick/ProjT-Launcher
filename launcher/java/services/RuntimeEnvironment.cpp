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

#include "java/services/RuntimeEnvironment.hpp"

#include <QDebug>

#define IBUS "@im=ibus"

namespace projt::java
{
	QString stripEnvEntries(const QString& name, const QString& value, const QString& remove)
	{
		QChar delimiter = ':';
#ifdef Q_OS_WIN32
		delimiter = ';';
#endif

		QStringList targetItems = value.split(delimiter);
		QStringList toRemove	= remove.split(delimiter);

		for (const QString& item : toRemove)
		{
			if (!targetItems.removeOne(item))
			{
				qWarning() << "Entry" << item << "could not be stripped from variable" << name;
			}
		}
		return targetItems.join(delimiter);
	}

	QProcessEnvironment buildCleanEnvironment()
	{
		QProcessEnvironment rawenv = QProcessEnvironment::systemEnvironment();
		QProcessEnvironment env;

		QStringList ignored = { "JAVA_ARGS", "CLASSPATH",	  "CONFIGPATH",	  "JAVA_HOME",
								"JRE_HOME",	 "_JAVA_OPTIONS", "JAVA_OPTIONS", "JAVA_TOOL_OPTIONS" };

		QStringList stripped = {
#if defined(Q_OS_LINUX) || defined(Q_OS_FREEBSD) || defined(Q_OS_OPENBSD)
			"LD_LIBRARY_PATH",
			"LD_PRELOAD",
#endif
			"QT_PLUGIN_PATH",
			"QT_FONTPATH"
		};

		for (const auto& key : rawenv.keys())
		{
			auto current = rawenv.value(key);
			if (ignored.contains(key))
			{
				qDebug() << "Env: ignoring" << key << current;
				continue;
			}
			if (key.startsWith("LAUNCHER_"))
			{
				qDebug() << "Env: ignoring" << key << current;
				continue;
			}
			if (stripped.contains(key))
			{
				QString cleaned = stripEnvEntries(key, current, rawenv.value("LAUNCHER_" + key));
				qDebug() << "Env: stripped" << key << current << "to" << cleaned;
				current = cleaned;
			}
#if defined(Q_OS_LINUX) || defined(Q_OS_FREEBSD) || defined(Q_OS_OPENBSD)
			if (key == "XMODIFIERS" && current.contains(IBUS))
			{
				QString saved = current;
				current.replace(IBUS, "");
				qDebug() << "Env: stripped" << IBUS << "from" << saved << ":" << current;
			}
#endif
			env.insert(key, current);
		}
#ifdef Q_OS_LINUX
		if (!env.contains("LD_LIBRARY_PATH"))
		{
			env.insert("LD_LIBRARY_PATH", "");
		}
#endif

		return env;
	}
} // namespace projt::java