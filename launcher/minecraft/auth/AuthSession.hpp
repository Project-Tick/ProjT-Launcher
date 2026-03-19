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
#include <memory>

#include "LaunchMode.h"

class MinecraftAccount;

struct AuthSession
{
	bool MakeOffline(QString offline_playername);
	void MakeDemo(QString name, QString uuid);

	QString serializeUserProperties();

	enum Status
	{
		Undetermined,
		RequiresOAuth,
		RequiresPassword,
		RequiresProfileSetup,
		PlayableOffline,
		PlayableOnline,
		GoneOrMigrated
	} status = Undetermined;

	// combined session ID
	QString session;
	// volatile auth token
	QString access_token;
	// profile name
	QString player_name;
	// profile ID
	QString uuid;
	// 'legacy' or 'mojang', depending on account type
	QString user_type;
	// The resolved launch mode for this session.
	LaunchMode launchMode = LaunchMode::Normal;
	// Did the auth server reply?
	bool auth_server_online = false;
	// Did the user request online mode?
	bool wants_online = true;

	// Is this a demo session?
	bool demo = false;
};

using AuthSessionPtr = std::shared_ptr<AuthSession>;
