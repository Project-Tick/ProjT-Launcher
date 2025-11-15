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

#include "AccountData.h"

namespace Parsers {
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
}  // namespace Parsers
