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

#include <QStyledItemDelegate>

/* Custom data types for our custom list models :) */
enum UserDataTypes
{
	TITLE		= 257, // QString
	DESCRIPTION = 258, // QString
	INSTALLED	= 259  // bool
};

/** This is an item delegate composed of:
 *  - An Icon on the left
 *  - A title
 *  - A description
 * */
class ProjectItemDelegate final : public QStyledItemDelegate
{
	Q_OBJECT

  public:
	ProjectItemDelegate(QWidget* parent);

	void paint(QPainter*, const QStyleOptionViewItem&, const QModelIndex&) const override;

	bool editorEvent(QEvent* event,
					 QAbstractItemModel* model,
					 const QStyleOptionViewItem& option,
					 const QModelIndex& index) override;

  signals:
	void checkboxClicked(const QModelIndex& index);

  private:
	QStyleOptionViewItem makeCheckboxStyleOption(const QStyleOptionViewItem& opt, const QStyle* style) const;
};
