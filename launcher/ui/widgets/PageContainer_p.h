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
 * Copyright 2013-2021 MultiMC Contributors
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

#include <QEvent>
#include <QListView>
#include <QScrollBar>
#include <QStyledItemDelegate>

class BasePage;
const int pageIconSize = 24;

class PageViewDelegate : public QStyledItemDelegate
{
  public:
	PageViewDelegate(QObject* parent) : QStyledItemDelegate(parent)
	{}
	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		QSize size = QStyledItemDelegate::sizeHint(option, index);
		size.setHeight(qMax(size.height(), 32));
		return size;
	}
};

class PageModel : public QAbstractListModel
{
  public:
	PageModel(QObject* parent = 0) : QAbstractListModel(parent)
	{
		QPixmap empty(pageIconSize, pageIconSize);
		empty.fill(Qt::transparent);
		m_emptyIcon = QIcon(empty);
	}
	virtual ~PageModel()
	{}

	int rowCount(const QModelIndex& parent = QModelIndex()) const
	{
		return parent.isValid() ? 0 : m_pages.size();
	}
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const
	{
		switch (role)
		{
			case Qt::DisplayRole: return m_pages.at(index.row())->displayName();
			case Qt::DecorationRole:
			{
				QIcon icon = m_pages.at(index.row())->icon();
				if (icon.isNull())
					icon = m_emptyIcon;
				// NOTE: Workaround for Qt icon stretching bug on Windows.
				return QIcon(icon.pixmap(QSize(48, 48)));
			}
		}
		return QVariant();
	}

	void setPages(const QList<BasePage*>& pages)
	{
		beginResetModel();
		m_pages = pages;
		endResetModel();
	}
	const QList<BasePage*>& pages() const
	{
		return m_pages;
	}

	BasePage* findPageEntryById(QString id)
	{
		for (auto page : m_pages)
		{
			if (page->id() == id)
				return page;
		}
		return nullptr;
	}

	QList<BasePage*> m_pages;
	QIcon m_emptyIcon;
};

class PageView : public QListView
{
  public:
	PageView(QWidget* parent = 0) : QListView(parent)
	{
		setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
		setItemDelegate(new PageViewDelegate(this));
		setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		// Adjust margins when using Breeze theme
		setProperty("_kde_side_panel_view", true);
	}

	virtual QSize sizeHint() const
	{
		int width = sizeHintForColumn(0) + frameWidth() * 2 + 5;
		if (verticalScrollBar()->isVisible())
			width += verticalScrollBar()->width();
		return QSize(width, 100);
	}

	virtual bool eventFilter(QObject* obj, QEvent* event)
	{
		if (obj == verticalScrollBar() && (event->type() == QEvent::Show || event->type() == QEvent::Hide))
			updateGeometry();
		return QListView::eventFilter(obj, event);
	}
};
