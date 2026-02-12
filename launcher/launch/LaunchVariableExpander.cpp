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

#include "LaunchVariableExpander.hpp"

namespace projt::launch
{
	QString LaunchVariableExpander::expand(const QString& input, const QProcessEnvironment& env) const
	{
		QString output = input;

		enum class Mode
		{
			Idle,
			Sigil,
			Name,
			Brace
		};

		Mode mode	   = Mode::Idle;
		int tokenStart = -1;

		for (int idx = 0; idx < output.length();)
		{
			const QChar c = output.at(idx++);
			switch (mode)
			{
				case Mode::Idle:
					if (c == '$')
					{
						mode = Mode::Sigil;
					}
					break;
				case Mode::Sigil:
					if (c == '{')
					{
						mode	   = Mode::Brace;
						tokenStart = idx;
					}
					else if (c.isLetterOrNumber() || c == '_')
					{
						mode	   = Mode::Name;
						tokenStart = idx - 1;
					}
					else
					{
						mode = Mode::Idle;
					}
					break;
				case Mode::Brace:
					if (c == '}')
					{
						const auto key	 = output.mid(tokenStart, idx - 1 - tokenStart);
						const auto value = env.value(key, "");
						if (!value.isEmpty())
						{
							output.replace(tokenStart - 2, idx - tokenStart + 2, value);
							idx = tokenStart - 2 + value.length();
						}
						mode = Mode::Idle;
					}
					break;
				case Mode::Name:
					if (!c.isLetterOrNumber() && c != '_')
					{
						const auto key	 = output.mid(tokenStart, idx - tokenStart - 1);
						const auto value = env.value(key, "");
						if (!value.isEmpty())
						{
							output.replace(tokenStart - 1, idx - tokenStart, value);
							idx = tokenStart - 1 + value.length();
						}
						mode = Mode::Idle;
					}
					break;
			}
		}

		if (mode == Mode::Name)
		{
			const auto value = env.value(output.mid(tokenStart), "");
			if (!value.isEmpty())
			{
				output.replace(tokenStart - 1, output.length() - tokenStart + 1, value);
			}
		}

		return output;
	}
} // namespace projt::launch
