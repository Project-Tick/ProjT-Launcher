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
#include "VanillaInstanceCreationTask.h"

#include <utility>

#include "FileSystem.h"
#include "minecraft/MinecraftInstance.h"
#include "minecraft/PackProfile.h"
#include "settings/INISettingsObject.h"

VanillaCreationTask::VanillaCreationTask(BaseVersion::Ptr version, QString loader, BaseVersion::Ptr loader_version)
	: InstanceCreationTask(),
	  m_version(std::move(version)),
	  m_using_loader(true),
	  m_loader(std::move(loader)),
	  m_loader_version(std::move(loader_version))
{}

std::unique_ptr<MinecraftInstance> VanillaCreationTask::createInstance()
{
	setStatus(tr("Creating instance from version %1").arg(m_version->name()));

	auto instance_settings = std::make_shared<INISettingsObject>(FS::PathCombine(m_stagingPath, "instance.cfg"));
	instance_settings->suspendSave();
	{
		auto createdInstance = std::make_unique<MinecraftInstance>(m_globalSettings, instance_settings, m_stagingPath);
		auto& inst		   = *createdInstance;
		auto components	   = inst.getPackProfile();
		components->buildingFromScratch();
		components->setComponentVersion("net.minecraft", m_version->descriptor(), true);
		if (m_using_loader)
			components->setComponentVersion(m_loader, m_loader_version->descriptor());

		inst.setName(name());
		inst.setIconKey(m_instIcon);

		instance_settings->resumeSave();
		return createdInstance;
	}
}
