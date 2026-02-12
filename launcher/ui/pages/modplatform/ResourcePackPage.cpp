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

#include "ResourcePackPage.h"
#include "ui_ResourcePage.h"

#include "ResourcePackModel.h"

#include "ui/dialogs/ResourceDownloadDialog.h"

#include <QRegularExpression>

namespace ResourceDownload
{

	ResourcePackResourcePage::ResourcePackResourcePage(ResourceDownloadDialog* dialog, BaseInstance& instance)
		: ResourcePage(dialog, instance)
	{}

	/******** Callbacks to events in the UI (set up in the derived classes) ********/

	void ResourcePackResourcePage::triggerSearch()
	{
		m_ui->packView->selectionModel()->setCurrentIndex({}, QItemSelectionModel::SelectionFlag::ClearAndSelect);
		m_ui->packView->clearSelection();
		m_ui->packDescription->clear();
		m_ui->versionSelectionBox->clear();

		updateSelectionButton();

		static_cast<ResourcePackResourceModel*>(m_model)->searchWithTerm(getSearchTerm(),
																		 m_ui->sortByBox->currentData().toUInt());
		m_fetchProgress.watch(m_model->activeSearchJob().get());
	}

	QMap<QString, QString> ResourcePackResourcePage::urlHandlers() const
	{
		QMap<QString, QString> map;
		map.insert(QRegularExpression::anchoredPattern("(?:www\\.)?modrinth\\.com\\/resourcepack\\/([^\\/]+)\\/?"),
				   "modrinth");
		map.insert(QRegularExpression::anchoredPattern(
					   "(?:www\\.)?curseforge\\.com\\/minecraft\\/texture-packs\\/([^\\/]+)\\/?"),
				   "curseforge");
		map.insert(QRegularExpression::anchoredPattern("minecraft\\.curseforge\\.com\\/projects\\/([^\\/]+)\\/?"),
				   "curseforge");
		return map;
	}

} // namespace ResourceDownload
