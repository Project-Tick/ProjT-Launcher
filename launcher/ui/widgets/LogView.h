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
#include <QAbstractItemView>
#include <QPlainTextEdit>

class QAbstractItemModel;

class LogView : public QPlainTextEdit
{
	Q_OBJECT
  public:
	explicit LogView(QWidget* parent = nullptr);
	virtual ~LogView();

	virtual void setModel(QAbstractItemModel* model);
	QAbstractItemModel* model() const;

  public slots:
	void setWordWrap(bool wrapping);
	void setColorLines(bool colorLines);
	void findNext(const QString& what, bool reverse);
	void scrollToBottom();

  protected slots:
	void repopulate();
	// note: this supports only appending
	void rowsInserted(const QModelIndex& parent, int first, int last);
	void rowsAboutToBeInserted(const QModelIndex& parent, int first, int last);
	// note: this supports only removing from front
	void rowsRemoved(const QModelIndex& parent, int first, int last);
	void modelDestroyed(QObject* model);

  protected:
	QAbstractItemModel* m_model		 = nullptr;
	QTextCharFormat* m_defaultFormat = nullptr;
	bool m_scroll					 = false;
	bool m_scrolling				 = false;
	bool m_colorLines				 = true;
};
