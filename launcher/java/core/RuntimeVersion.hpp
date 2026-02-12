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

#include <QString>

#ifdef major
#undef major
#endif
#ifdef minor
#undef minor
#endif

namespace projt::java
{
	class RuntimeVersion
	{
		friend class RuntimeVersionTest;

	  public:
		RuntimeVersion() = default;
		explicit RuntimeVersion(const QString& raw);
		RuntimeVersion(int major, int minor, int security, int build = 0, QString name = "");

		RuntimeVersion& operator=(const QString& raw);

		bool operator<(const RuntimeVersion& rhs) const;
		bool operator==(const RuntimeVersion& rhs) const;
		bool operator>(const RuntimeVersion& rhs) const;

		bool needsPermGen() const;
		bool defaultsToUtf8() const;
		bool supportsModules() const;

		QString toString() const;

		int major() const
		{
			return m_major;
		}
		int minor() const
		{
			return m_minor;
		}
		int security() const
		{
			return m_security;
		}
		QString prerelease() const
		{
			return m_prerelease;
		}
		QString name() const
		{
			return m_name;
		}

	  private:
		void parse(const QString& raw);

		QString m_raw;
		int m_major	   = 0;
		int m_minor	   = 0;
		int m_security = 0;
		QString m_name;
		bool m_valid = false;
		QString m_prerelease;
	};
} // namespace projt::java