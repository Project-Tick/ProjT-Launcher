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

#include "AccountData.hpp"

namespace Parsers
{
	bool getDateTime(QJsonValue value, QDateTime& out);
	bool getString(QJsonValue value, QString& out);
	bool getNumber(QJsonValue value, double& out);
	bool getNumber(QJsonValue value, int64_t& out);
	bool getBool(QJsonValue value, bool& out);

	bool parseXTokenResponse(QByteArray& data, Token& output, QString name);
	bool parseMojangResponse(QByteArray& data, Token& output);

	bool parseMinecraftProfile(QByteArray& data, MinecraftProfile& output);
	bool parseMinecraftProfileMojang(QByteArray& data, MinecraftProfile& output);
	bool parseMinecraftEntitlements(QByteArray& data, MinecraftEntitlement& output);
	bool parseRolloutResponse(QByteArray& data, bool& result);
} // namespace Parsers
