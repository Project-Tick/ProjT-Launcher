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

#include "java/core/RuntimeInstall.hpp"

#include "BaseVersion.h"
#include "StringUtils.h"

#include <typeinfo>

namespace projt::java
{
	bool RuntimeInstall::operator<(const RuntimeInstall& rhs) const
	{
		if (version < rhs.version)
		{
			return true;
		}
		if (version > rhs.version)
		{
			return false;
		}
		if (is_64bit != rhs.is_64bit)
		{
			return rhs.is_64bit;
		}
		auto archCompare = StringUtils::naturalCompare(arch, rhs.arch, Qt::CaseInsensitive);
		if (archCompare != 0)
		{
			return archCompare < 0;
		}
		return StringUtils::naturalCompare(path, rhs.path, Qt::CaseInsensitive) < 0;
	}

	bool RuntimeInstall::operator==(const RuntimeInstall& rhs) const
	{
		return version == rhs.version && arch == rhs.arch && path == rhs.path && is_64bit == rhs.is_64bit;
	}

	bool RuntimeInstall::operator>(const RuntimeInstall& rhs) const
	{
		return (!operator<(rhs)) && (!operator==(rhs));
	}

	bool RuntimeInstall::operator<(BaseVersion& a) const
	{
		try
		{
			return operator<(dynamic_cast<RuntimeInstall&>(a));
		}
		catch (const std::bad_cast&)
		{
			return BaseVersion::operator<(a);
		}
	}

	bool RuntimeInstall::operator>(BaseVersion& a) const
	{
		try
		{
			return operator>(dynamic_cast<RuntimeInstall&>(a));
		}
		catch (const std::bad_cast&)
		{
			return BaseVersion::operator>(a);
		}
	}
} // namespace projt::java
