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
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (c) 2023 Trial97 <alexandru.tripon97@gmail.com>
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
 * ======================================================================== */

#pragma once

#include <QAbstractListModel>
#include <QIcon>
#include <QSortFilterProxyModel>
#include <QVariant>
#include "modplatform/import_ftb/PackHelpers.h"

namespace FTBImportAPP
{

	class FilterModel : public QSortFilterProxyModel
	{
		Q_OBJECT
	  public:
		FilterModel(QObject* parent = Q_NULLPTR);
		enum Sorting
		{
			ByName,
			ByGameVersion
		};
		const QMap<QString, Sorting> getAvailableSortings();
		QString translateCurrentSorting();
		void setSorting(Sorting sorting);
		Sorting getCurrentSorting();
		void setSearchTerm(QString term);

	  protected:
		bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;
		bool lessThan(const QModelIndex& left, const QModelIndex& right) const override;

	  private:
		QMap<QString, Sorting> m_sortings;
		Sorting m_currentSorting;
		QString m_searchTerm;
	};

	class ListModel : public QAbstractListModel
	{
		Q_OBJECT

	  public:
		ListModel(QObject* parent);
		virtual ~ListModel() = default;

		int rowCount([[maybe_unused]] const QModelIndex& parent) const
		{
			return m_modpacks.size();
		}
		int columnCount([[maybe_unused]] const QModelIndex& parent) const
		{
			return 1;
		}
		QVariant data(const QModelIndex& index, int role) const;

		void update();

		QString getUserPath();
		void setPath(QString path);

	  private:
		void ensurePathInitialized();
		ModpackList m_modpacks;
		QString m_instances_path;
	};
} // namespace FTBImportAPP