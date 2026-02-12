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
#include "java/core/RuntimeVersion.hpp"

#include "StringUtils.h"

#include <QStringList>

namespace projt::java
{
	RuntimeVersion::RuntimeVersion(const QString& raw)
	{
		parse(raw);
	}

	RuntimeVersion::RuntimeVersion(int major, int minor, int security, int build, QString name)
		: m_major(major),
		  m_minor(minor),
		  m_security(security),
		  m_name(name),
		  m_valid(true)
	{
		QStringList parts;
		if (build != 0)
		{
			m_prerelease = QString::number(build);
			parts.push_front(m_prerelease);
		}
		if (m_security != 0)
		{
			parts.push_front(QString::number(m_security));
		}
		else if (!parts.isEmpty())
		{
			parts.push_front("0");
		}

		if (m_minor != 0)
		{
			parts.push_front(QString::number(m_minor));
		}
		else if (!parts.isEmpty())
		{
			parts.push_front("0");
		}
		parts.push_front(QString::number(m_major));
		m_raw = parts.join(".");
	}

	RuntimeVersion& RuntimeVersion::operator=(const QString& raw)
	{
		parse(raw);
		return *this;
	}

	void RuntimeVersion::parse(const QString& raw)
	{
		m_raw	   = raw.trimmed();
		m_major	   = 0;
		m_minor	   = 0;
		m_security = 0;
		m_prerelease.clear();
		m_valid = false;

		QString numeric = m_raw;
		auto dashIndex	= numeric.indexOf('-');
		if (dashIndex >= 0)
		{
			m_prerelease = numeric.mid(dashIndex + 1).trimmed();
			numeric		 = numeric.left(dashIndex);
		}

		int updateNumber = 0;
		auto updateIndex = numeric.indexOf('_');
		if (updateIndex >= 0)
		{
			bool ok		 = false;
			updateNumber = numeric.mid(updateIndex + 1).toInt(&ok);
			if (!ok)
			{
				updateNumber = 0;
			}
			numeric = numeric.left(updateIndex);
		}

		numeric.replace('_', '.');
		QStringList parts = numeric.split('.', Qt::SkipEmptyParts);
		if (!parts.isEmpty() && parts[0] == "1" && parts.size() > 1)
		{
			parts.removeFirst();
		}
		if (parts.isEmpty())
		{
			return;
		}

		auto parsePart = [](const QString& value, int& out)
		{
			bool ok	   = false;
			int parsed = value.toInt(&ok);
			if (!ok)
			{
				return false;
			}
			out = parsed;
			return true;
		};

		if (!parsePart(parts[0], m_major))
		{
			return;
		}
		m_valid = true;
		if (parts.size() > 1)
		{
			parsePart(parts[1], m_minor);
		}
		if (parts.size() > 2)
		{
			parsePart(parts[2], m_security);
		}
		if (updateNumber != 0)
		{
			m_security = updateNumber;
		}
	}

	QString RuntimeVersion::toString() const
	{
		return m_raw;
	}

	bool RuntimeVersion::needsPermGen() const
	{
		return !m_valid || m_major <= 7;
	}

	bool RuntimeVersion::defaultsToUtf8() const
	{
		return m_valid && m_major >= 18;
	}

	bool RuntimeVersion::supportsModules() const
	{
		return m_valid && m_major >= 9;
	}

	bool RuntimeVersion::operator<(const RuntimeVersion& rhs) const
	{
		if (m_valid && rhs.m_valid)
		{
			if (m_major != rhs.m_major)
			{
				return m_major < rhs.m_major;
			}
			if (m_minor != rhs.m_minor)
			{
				return m_minor < rhs.m_minor;
			}
			if (m_security != rhs.m_security)
			{
				return m_security < rhs.m_security;
			}

			bool thisPre = !m_prerelease.isEmpty();
			bool rhsPre	 = !rhs.m_prerelease.isEmpty();
			if (thisPre != rhsPre)
			{
				return thisPre;
			}
			if (thisPre && rhsPre)
			{
				return StringUtils::naturalCompare(m_prerelease, rhs.m_prerelease, Qt::CaseSensitive) < 0;
			}
			return false;
		}
		return StringUtils::naturalCompare(m_raw, rhs.m_raw, Qt::CaseSensitive) < 0;
	}

	bool RuntimeVersion::operator==(const RuntimeVersion& rhs) const
	{
		if (m_valid && rhs.m_valid)
		{
			return m_major == rhs.m_major && m_minor == rhs.m_minor && m_security == rhs.m_security
				&& m_prerelease == rhs.m_prerelease;
		}
		return m_raw == rhs.m_raw;
	}

	bool RuntimeVersion::operator>(const RuntimeVersion& rhs) const
	{
		return (!operator<(rhs)) && (!operator==(rhs));
	}
} // namespace projt::java
