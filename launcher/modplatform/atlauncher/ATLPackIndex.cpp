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
 * Copyright 2020-2021 Jamie Mansfield <jmansfield@cadixdev.org>
 * Copyright 2021 Petr Mrazek <peterix@gmail.com>
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

#include "ATLPackIndex.h"

#include <QRegularExpression>

#include "Json.h"

static void loadIndexedVersion(ATLauncher::IndexedVersion& v, QJsonObject& obj)
{
	v.version	= Json::requireString(obj, "version");
	v.minecraft = Json::requireString(obj, "minecraft");
}

void ATLauncher::loadIndexedPack(ATLauncher::IndexedPack& m, QJsonObject& obj)
{
	m.id	   = Json::requireInteger(obj, "id");
	m.position = Json::requireInteger(obj, "position");
	m.name	   = Json::requireString(obj, "name");
	m.type =
		Json::requireString(obj, "type") == "private" ? ATLauncher::PackType::Private : ATLauncher::PackType::Public;
	auto versionsArr = Json::requireArray(obj, "versions");
	for (const auto versionRaw : versionsArr)
	{
		auto versionObj = Json::requireObject(versionRaw);
		ATLauncher::IndexedVersion version;
		loadIndexedVersion(version, versionObj);
		m.versions.append(version);
	}
	m.system	  = Json::ensureBoolean(obj, QString("system"), false);
	m.description = Json::ensureString(obj, "description", "");

	static const QRegularExpression s_regex("[^A-Za-z0-9]");
	m.safeName = Json::requireString(obj, "name").replace(s_regex, "").toLower() + ".png";
}
