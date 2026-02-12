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
 */
#pragma once

#include "ResourceFolderModel.hpp"

#include "ResourcePack.hpp"

class ResourcePackFolderModel : public ResourceFolderModel
{
	Q_OBJECT
  public:
	enum Columns
	{
		ActiveColumn = 0,
		ImageColumn,
		NameColumn,
		PackFormatColumn,
		DateColumn,
		ProviderColumn,
		SizeColumn,
		NUM_COLUMNS
	};

	explicit ResourcePackFolderModel(const QDir& dir,
									 BaseInstance* instance,
									 bool is_indexed,
									 bool create_dir,
									 QObject* parent = nullptr);

	QString id() const override
	{
		return "resourcepacks";
	}

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	int columnCount(const QModelIndex& parent) const override;

	[[nodiscard]] Resource* createResource(const QFileInfo& file) override
	{
		return new ResourcePack(file);
	}
	[[nodiscard]] Task* createParseTask(Resource&) override;

	RESOURCE_HELPERS(ResourcePack)
};
