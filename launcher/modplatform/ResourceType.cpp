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
 *
 * === Upstream License Block (Do Not Modify) ==============================
 *
 *
 *
 *
 *
 *
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (C) 2022 Rachel Powers <508861+Ryex@users.noreply.github.com>
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
 *
 *
 * ======================================================================== */

#include "ResourceType.h"

namespace ModPlatform
{
	static const QMap<ResourceType, QString> s_packedTypeNames = {
		{ ResourceType::ResourcePack, QObject::tr("resource pack") },
		{ ResourceType::TexturePack, QObject::tr("texture pack") },
		{ ResourceType::DataPack, QObject::tr("data pack") },
		{ ResourceType::ShaderPack, QObject::tr("shader pack") },
		{ ResourceType::World, QObject::tr("world save") },
		{ ResourceType::Mod, QObject::tr("mod") },
		{ ResourceType::Unknown, QObject::tr("unknown") }
	};

	namespace ResourceTypeUtils
	{

		QString getName(ResourceType type)
		{
			const auto typeIt = s_packedTypeNames.constFind(type);
			if (typeIt != s_packedTypeNames.cend())
				return *typeIt;
			return s_packedTypeNames.value(ResourceType::Unknown);
		}

	} // namespace ResourceTypeUtils
} // namespace ModPlatform
