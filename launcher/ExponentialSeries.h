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
inline void clamp(T& current, T min, T max)
{
	if (current < min)
	{
		current = min;
	}
	else if (current > max)
	{
		current = max;
	}
}

// List of numbers from min to max. Next is exponent times bigger than previous.

class ExponentialSeries
{
  public:
	ExponentialSeries(unsigned min, unsigned max, unsigned exponent = 2)
	{
		m_current = m_min = min;
		m_max			  = max;
		m_exponent		  = exponent;
	}
	void reset()
	{
		m_current = m_min;
	}
	unsigned operator()()
	{
		unsigned retval = m_current;
		m_current *= m_exponent;
		clamp(m_current, m_min, m_max);
		return retval;
	}
	unsigned m_current;
	unsigned m_min;
	unsigned m_max;
	unsigned m_exponent;
};
