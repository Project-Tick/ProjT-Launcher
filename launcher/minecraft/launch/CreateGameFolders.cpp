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
#include "CreateGameFolders.h"
#include "FileSystem.h"
#include "launch/LaunchTask.h"
#include "minecraft/MinecraftInstance.h"

CreateGameFolders::CreateGameFolders(LaunchTask* parent) : LaunchStep(parent) {}

void CreateGameFolders::executeTask()
{
    auto instance = m_parent->instance();

    if (!FS::ensureFolderPathExists(instance->gameRoot())) {
        emit logLine("Couldn't create the main game folder", MessageLevel::Error);
        emitFailed(tr("Couldn't create the main game folder"));
        return;
    }

    // HACK: this is a workaround for MCL-3732 - 'server-resource-packs' folder is created.
    if (!FS::ensureFolderPathExists(FS::PathCombine(instance->gameRoot(), "server-resource-packs"))) {
        emit logLine("Couldn't create the 'server-resource-packs' folder", MessageLevel::Error);
    }
    emitSucceeded();
}
