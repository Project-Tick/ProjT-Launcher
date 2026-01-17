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
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "AuthSession.hpp"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringList>

QString AuthSession::serializeUserProperties()
{
	QJsonObject userAttrs;
	/*
	for (auto key : u.properties.keys())
	{
		auto array = QJsonArray::fromStringList(u.properties.values(key));
		userAttrs.insert(key, array);
	}
	*/
	QJsonDocument value(userAttrs);
	return value.toJson(QJsonDocument::Compact);
}

bool AuthSession::MakeOffline(QString offline_playername)
{
	if (status != PlayableOffline && status != PlayableOnline)
	{
		return false;
	}
	session		 = "-";
	access_token = "0";
	player_name	 = offline_playername;
	status		 = PlayableOffline;
	return true;
}

void AuthSession::MakeDemo(QString name, QString u)
{
	wants_online = false;
	demo		 = true;
	uuid		 = u;
	session		 = "-";
	access_token = "0";
	player_name	 = name;
	status		 = PlayableOnline; // needs online to download the assets
};