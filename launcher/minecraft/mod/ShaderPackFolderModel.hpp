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

#include "ResourceFolderModel.hpp"
#include "minecraft/mod/ShaderPack.hpp"
#include "minecraft/mod/tasks/LocalShaderPackParseTask.hpp"

class ShaderPackFolderModel : public ResourceFolderModel
{
	Q_OBJECT

  public:
	explicit ShaderPackFolderModel(const QDir& dir,
								   BaseInstance* instance,
								   bool is_indexed,
								   bool create_dir,
								   QObject* parent = nullptr)
		: ResourceFolderModel(dir, instance, is_indexed, create_dir, parent)
	{}

	virtual QString id() const override
	{
		return "shaderpacks";
	}

	[[nodiscard]] Resource* createResource(const QFileInfo& info) override
	{
		return new ShaderPack(info);
	}

	[[nodiscard]] Task* createParseTask(Resource& resource) override
	{
		return new LocalShaderPackParseTask(m_next_resolution_ticket, static_cast<ShaderPack&>(resource));
	}

	RESOURCE_HELPERS(ShaderPack);
};
