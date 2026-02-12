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

#include <QAbstractListModel>

#include "BaseInstance.h"

#include "modplatform/ModIndex.h"

#include "ui/pages/modplatform/ResourceModel.h"

class Version;

namespace ResourceDownload
{

	class ResourcePackResourceModel : public ResourceModel
	{
		Q_OBJECT

	  public:
		ResourcePackResourceModel(BaseInstance const&, ResourceAPI*, QString debugName, QString metaEntryBase);

		/* Ask the API for more information */
		void searchWithTerm(const QString& term, unsigned int sort);

		[[nodiscard]] QString debugName() const override
		{
			return m_debugName;
		}
		[[nodiscard]] QString metaEntryBase() const override
		{
			return m_metaEntryBase;
		}

	  public slots:
		ResourceAPI::SearchArgs createSearchArguments() override;
		ResourceAPI::VersionSearchArgs createVersionsArguments(const QModelIndex&) override;
		ResourceAPI::ProjectInfoArgs createInfoArguments(const QModelIndex&) override;

	  protected:
		const BaseInstance& m_base_instance;

	  private:
		QString m_debugName;
		QString m_metaEntryBase;
	};

} // namespace ResourceDownload
