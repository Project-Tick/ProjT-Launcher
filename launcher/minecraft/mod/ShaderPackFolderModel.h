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

#include "ResourceFolderModel.h"
#include "minecraft/mod/ShaderPack.h"
#include "minecraft/mod/tasks/LocalShaderPackParseTask.h"

class ShaderPackFolderModel : public ResourceFolderModel {
    Q_OBJECT

   public:
    explicit ShaderPackFolderModel(const QDir& dir, BaseInstance* instance, bool is_indexed, bool create_dir, QObject* parent = nullptr)
        : ResourceFolderModel(dir, instance, is_indexed, create_dir, parent)
    {}

    virtual QString id() const override { return "shaderpacks"; }

    [[nodiscard]] Resource* createResource(const QFileInfo& info) override { return new ShaderPack(info); }

    [[nodiscard]] Task* createParseTask(Resource& resource) override
    {
        return new LocalShaderPackParseTask(m_next_resolution_ticket, static_cast<ShaderPack&>(resource));
    }

    RESOURCE_HELPERS(ShaderPack);
};
