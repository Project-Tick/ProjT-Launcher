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

#include <cstddef>
#include <memory>

#include "QObjectPtr.h"

class Usable;

/**
 * Base class for things that can be used by multiple other things and we want to track the use count.
 *
 * @see UseLock
 */
class Usable
{
	friend class UseLock;

  public:
	virtual ~Usable()
	{}

	std::size_t useCount() const
	{
		return m_useCount;
	}
	bool isInUse() const
	{
		return m_useCount > 0;
	}

  protected:
	virtual void decrementUses()
	{
		m_useCount--;
	}
	virtual void incrementUses()
	{
		m_useCount++;
	}

  private:
	std::size_t m_useCount = 0;
};

/**
 * Lock class to use for keeping track of uses of other things derived from Usable
 *
 * @see Usable
 */
class UseLock
{
  public:
	UseLock(shared_qobject_ptr<Usable> usable) : m_usable(usable)
	{
		// this doesn't use shared pointer use count, because that wouldn't be correct. this count is separate.
		m_usable->incrementUses();
	}
	~UseLock()
	{
		m_usable->decrementUses();
	}

  private:
	shared_qobject_ptr<Usable> m_usable;
};
