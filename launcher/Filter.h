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

#include <QRegularExpression>
#include <QString>

using Filter = std::function<bool(const QString&)>;

namespace Filters
{
	inline Filter inverse(Filter filter)
	{
		return [filter = std::move(filter)](const QString& src) { return !filter(src); };
	}

	inline Filter any(QList<Filter> filters)
	{
		return [filters = std::move(filters)](const QString& src)
		{
			for (auto& filter : filters)
				if (filter(src))
					return true;

			return false;
		};
	}

	inline Filter equals(QString pattern)
	{
		return [pattern = std::move(pattern)](const QString& src) { return src == pattern; };
	}

	inline Filter equalsAny(QStringList patterns = {})
	{
		return [patterns = std::move(patterns)](const QString& src)
		{ return patterns.isEmpty() || patterns.contains(src); };
	}

	inline Filter equalsOrEmpty(QString pattern)
	{
		return [pattern = std::move(pattern)](const QString& src) { return src.isEmpty() || src == pattern; };
	}

	inline Filter contains(QString pattern)
	{
		return [pattern = std::move(pattern)](const QString& src) { return src.contains(pattern); };
	}

	inline Filter startsWith(QString pattern)
	{
		return [pattern = std::move(pattern)](const QString& src) { return src.startsWith(pattern); };
	}

	inline Filter regexp(QRegularExpression pattern)
	{
		return [pattern = std::move(pattern)](const QString& src) { return pattern.match(src).hasMatch(); };
	}
} // namespace Filters
