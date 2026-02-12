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
 * ======================================================================== */

#include "FlameResourceModels.h"

#include "Json.h"

#include "minecraft/PackProfile.h"
#include "modplatform/flame/FlameAPI.h"
#include "ui/pages/modplatform/flame/FlameResourcePages.h"

namespace ResourceDownload
{

	static bool isOptedOut(const ModPlatform::IndexedVersion& ver)
	{
		return ver.downloadUrl.isEmpty();
	}

	FlameTexturePackModel::FlameTexturePackModel(const BaseInstance& base)
		: TexturePackResourceModel(base, new FlameAPI, Flame::debugName(), Flame::metaEntryBase())
	{}

	ResourceAPI::SearchArgs FlameTexturePackModel::createSearchArguments()
	{
		auto args = TexturePackResourceModel::createSearchArguments();

		auto profile					   = static_cast<const MinecraftInstance&>(m_base_instance).getPackProfile();
		QString instance_minecraft_version = profile->getComponentVersion("net.minecraft");

		// Bypass the texture pack logic, because we can't do multiple versions in the API query
		args.versions = { instance_minecraft_version };

		return args;
	}

	ResourceAPI::VersionSearchArgs FlameTexturePackModel::createVersionsArguments(const QModelIndex& entry)
	{
		auto args = TexturePackResourceModel::createVersionsArguments(entry);

		// Bypass the texture pack logic, because we can't do multiple versions in the API query
		args.mcVersions = {};

		return args;
	}

	bool FlameTexturePackModel::optedOut(const ModPlatform::IndexedVersion& ver) const
	{
		return isOptedOut(ver);
	}

} // namespace ResourceDownload
