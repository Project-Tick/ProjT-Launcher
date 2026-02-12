// SPDX-License-Identifier: GPL-3.0-only AND Apache-2.0
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
 *
 * === Upstream License Block (Do Not Modify) ==============================
 *
 * Copyright 2013-2021 MultiMC Contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ======================================================================== */

#include "MinecraftTarget.hpp"

#include <QRegularExpression>
#include <QStringList>

// Note: This parser intentionally mirrors Minecraft's address parsing behavior exactly,
// including its tolerance for malformed input. Invalid addresses resolve to unusable
// targets, which Minecraft handles at connection time. The isValid() method can be
// used by callers requiring validation.
MinecraftTarget MinecraftTarget::parse(const QString& fullAddress, bool useWorld)
{
	// Validate input - empty or whitespace-only addresses are invalid
	QString trimmed = fullAddress.trimmed();
	if (trimmed.isEmpty())
	{
		return MinecraftTarget{}; // Return empty target for invalid input
	}

	if (useWorld)
	{
		MinecraftTarget target;
		target.world = trimmed;
		return target;
	}

	QStringList split = trimmed.split(":");

	// The logic below replicates the exact logic minecraft uses for parsing server addresses.
	// While the conversion is not lossless and eats errors, it ensures the same behavior
	// within Minecraft and ProjT Launcher when entering server addresses.
	if (trimmed.startsWith("["))
	{
		int bracket = trimmed.indexOf("]");
		if (bracket > 0)
		{
			QString ipv6 = trimmed.mid(1, bracket - 1);
			QString port = trimmed.mid(bracket + 1).trimmed();

			if (port.startsWith(":") && !ipv6.isEmpty())
			{
				port  = port.mid(1);
				split = QStringList({ ipv6, port });
			}
			else
			{
				split = QStringList({ ipv6 });
			}
		}
	}

	if (split.size() > 2)
	{
		split = QStringList({ trimmed });
	}

	QString realAddress = split[0];

	// Validate address is not empty after parsing
	if (realAddress.isEmpty())
	{
		return MinecraftTarget{}; // Invalid address
	}

	quint16 realPort = 25565;
	if (split.size() > 1)
	{
		bool ok;
		uint portValue = split[1].toUInt(&ok);

		// Validate port is in valid range (1-65535)
		if (ok && portValue > 0 && portValue <= 65535)
		{
			realPort = static_cast<quint16>(portValue);
		}
		else
		{
			// Invalid port, use default
			realPort = 25565;
		}
	}

	return MinecraftTarget{ realAddress, realPort };
}
