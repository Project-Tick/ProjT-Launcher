// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  This file is part of ProjT Launcher and is licensed under
 *  the GNU General Public License version 3 or later.
 *
 *  If this file includes work from previous open-source projects,
 *  their original copyright and license notices are preserved below.
 */
#pragma once

#include <QString>

#include "Library.h"

class Agent;

using AgentPtr = std::shared_ptr<Agent>;

class Agent {
   public:
    Agent(LibraryPtr library, const QString& argument)
    {
        m_library = library;
        m_argument = argument;
    }

   public: /* methods */
    LibraryPtr library() { return m_library; }
    QString argument() { return m_argument; }

   protected: /* data */
    /// The library pointing to the jar this Java agent is contained within
    LibraryPtr m_library;

    /// The argument to the Java agent, passed after an = if present
    QString m_argument;
};
