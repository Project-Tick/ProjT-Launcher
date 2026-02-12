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

#include "Library.h"

class Agent;

using AgentPtr = std::shared_ptr<Agent>;

class Agent
{
  public:
	Agent(LibraryPtr library, const QString& argument)
	{
		m_library  = library;
		m_argument = argument;
	}

  public: /* methods */
	LibraryPtr library()
	{
		return m_library;
	}
	QString argument()
	{
		return m_argument;
	}

  protected: /* data */
	/// The library pointing to the jar this Java agent is contained within
	LibraryPtr m_library;

	/// The argument to the Java agent, passed after an = if present
	QString m_argument;
};
