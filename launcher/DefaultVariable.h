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

template <typename T>
class DefaultVariable
{
  public:
	DefaultVariable(const T& value)
	{
		defaultValue = value;
	}
	DefaultVariable<T>& operator=(const T& value)
	{
		currentValue = value;
		is_default	 = currentValue == defaultValue;
		is_explicit	 = true;
		return *this;
	}
	operator const T&() const
	{
		return is_default ? defaultValue : currentValue;
	}
	bool isDefault() const
	{
		return is_default;
	}
	bool isExplicit() const
	{
		return is_explicit;
	}

  private:
	T currentValue;
	T defaultValue;
	bool is_default	 = true;
	bool is_explicit = false;
};
