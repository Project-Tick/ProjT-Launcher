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

#include "ui/pages/modplatform/ModModel.h"
#include "ui/pages/modplatform/flame/FlameResourcePages.h"

namespace ResourceDownload
{

	class FlameTexturePackModel : public TexturePackResourceModel
	{
		Q_OBJECT

	  public:
		FlameTexturePackModel(const BaseInstance&);
		~FlameTexturePackModel() override = default;

		bool optedOut(const ModPlatform::IndexedVersion& ver) const override;

	  private:
		QString debugName() const override
		{
			return Flame::debugName() + " (Model)";
		}
		QString metaEntryBase() const override
		{
			return Flame::metaEntryBase();
		}

		ResourceAPI::SearchArgs createSearchArguments() override;
		ResourceAPI::VersionSearchArgs createVersionsArguments(const QModelIndex&) override;
	};

} // namespace ResourceDownload
