// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
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
 *
 * === Upstream License Block (Do Not Modify) ==============================
 *
 * // SPDX-FileCopyrightText: 2023 flowln <flowlnlnln@gmail.com>
 * //
 * // SPDX-License-Identifier: GPL-3.0-only
 *
 * ======================================================================== */

#include "ResourcePackModel.h"

#include <QMessageBox>

namespace ResourceDownload {

ResourcePackResourceModel::ResourcePackResourceModel(BaseInstance const& base_inst,
                                                     ResourceAPI* api,
                                                     QString debugName,
                                                     QString metaEntryBase)
    : ResourceModel(api), m_base_instance(base_inst), m_debugName(debugName + " (Model)"), m_metaEntryBase(metaEntryBase)
{}

/******** Make data requests ********/

ResourceAPI::SearchArgs ResourcePackResourceModel::createSearchArguments()
{
    auto sort = getCurrentSortingMethodByIndex();
    return { ModPlatform::ResourceType::ResourcePack, m_next_search_offset, m_search_term, sort };
}

ResourceAPI::VersionSearchArgs ResourcePackResourceModel::createVersionsArguments(const QModelIndex& entry)
{
    auto pack = m_packs[entry.row()];
    return { pack, {}, {}, ModPlatform::ResourceType::ResourcePack };
}

ResourceAPI::ProjectInfoArgs ResourcePackResourceModel::createInfoArguments(const QModelIndex& entry)
{
    auto pack = m_packs[entry.row()];
    return { pack };
}

void ResourcePackResourceModel::searchWithTerm(const QString& term, unsigned int sort)
{
    if (m_search_term == term && m_search_term.isNull() == term.isNull() && m_current_sort_index == sort) {
        return;
    }

    setSearchTerm(term);
    m_current_sort_index = sort;

    refresh();
}

}  // namespace ResourceDownload
