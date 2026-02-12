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

#pragma once

#include "ui/pages/modplatform/ResourcePackModel.h"
#include "ui/pages/modplatform/ResourcePage.h"

namespace Ui
{
	class ResourcePage;
}

namespace ResourceDownload
{

	class ResourcePackDownloadDialog;

	class ResourcePackResourcePage : public ResourcePage
	{
		Q_OBJECT

	  public:
		template <typename T>
		static T* create(ResourcePackDownloadDialog* dialog, BaseInstance& instance)
		{
			auto page  = new T(dialog, instance);
			auto model = static_cast<ResourcePackResourceModel*>(page->getModel());

			connect(model, &ResourceModel::versionListUpdated, page, &ResourcePage::versionListUpdated);
			connect(model, &ResourceModel::projectInfoUpdated, page, &ResourcePage::updateUi);
			connect(model, &QAbstractListModel::modelReset, page, &ResourcePage::modelReset);

			return page;
		}

		//: The plural version of 'resource pack'
		inline QString resourcesString() const override
		{
			return tr("resource packs");
		}
		//: The singular version of 'resource packs'
		inline QString resourceString() const override
		{
			return tr("resource pack");
		}

		bool supportsFiltering() const override
		{
			return false;
		};

		QMap<QString, QString> urlHandlers() const override;

		inline auto helpPage() const -> QString override
		{
			return "resourcepack-platform";
		}

	  protected:
		ResourcePackResourcePage(ResourceDownloadDialog* dialog, BaseInstance& instance);

	  protected slots:
		void triggerSearch() override;
	};

} // namespace ResourceDownload
