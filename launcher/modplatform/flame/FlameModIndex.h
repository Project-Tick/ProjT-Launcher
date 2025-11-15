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

#include "modplatform/ModIndex.h"

#include "BaseInstance.h"

namespace FlameMod {

void loadIndexedPack(ModPlatform::IndexedPack& m, QJsonObject& obj);
void loadURLs(ModPlatform::IndexedPack& m, QJsonObject& obj);
void loadBody(ModPlatform::IndexedPack& m);
void loadIndexedPackVersions(ModPlatform::IndexedPack& pack, QJsonArray& arr);
ModPlatform::IndexedVersion loadIndexedPackVersion(QJsonObject& obj, bool load_changelog = false);
}  // namespace FlameMod