// SPDX-License-Identifier: GPL-3.0-only AND Apache-2.0
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
 * Copyright 2020-2021 Jamie Mansfield <jmansfield@cadixdev.org>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ======================================================================== */

#pragma once

#include <QtCore/QSortFilterProxyModel>

namespace Atl
{

	class FilterModel : public QSortFilterProxyModel
	{
		Q_OBJECT
	  public:
		FilterModel(QObject* parent = Q_NULLPTR);
		enum Sorting
		{
			ByPopularity,
			ByGameVersion,
			ByName,
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
		QMap<QString, Sorting> sortings;
		Sorting currentSorting;
		QString searchTerm;
	};

} // namespace Atl
