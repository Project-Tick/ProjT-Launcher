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
 *
 * === Upstream License Block (Do Not Modify) ==============================
 *
 * // SPDX-FileCopyrightText: 2023 flowln <flowlnlnln@gmail.com>
 * //
 * // SPDX-License-Identifier: GPL-3.0-only
 *
 * ======================================================================== */

#include "FlameResourceModels.h"

#include "Json.h"

#include "minecraft/PackProfile.h"
#include "modplatform/flame/FlameAPI.h"
#include "ui/pages/modplatform/flame/FlameResourcePages.h"

namespace ResourceDownload {

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

    auto profile = static_cast<const MinecraftInstance&>(m_base_instance).getPackProfile();
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

}  // namespace ResourceDownload
