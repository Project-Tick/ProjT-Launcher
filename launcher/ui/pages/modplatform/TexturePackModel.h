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

#pragma once

#include "meta/VersionList.h"
#include "ui/pages/modplatform/ResourcePackModel.h"

namespace ResourceDownload {

class TexturePackResourceModel : public ResourcePackResourceModel {
    Q_OBJECT

   public:
    TexturePackResourceModel(BaseInstance const& inst, ResourceAPI* api, QString debugName, QString metaEntryBase);

    inline ::Version maximumTexturePackVersion() const { return { "1.6" }; }

    ResourceAPI::SearchArgs createSearchArguments() override;
    ResourceAPI::VersionSearchArgs createVersionsArguments(const QModelIndex&) override;

   protected:
    Meta::VersionList::Ptr m_version_list;
    Task::Ptr m_task;
};

}  // namespace ResourceDownload
