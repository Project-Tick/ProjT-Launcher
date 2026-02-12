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
#include <QTreeView>

class VersionListView : public QTreeView
{
	Q_OBJECT
  public:
	explicit VersionListView(QWidget* parent = 0);
	virtual void paintEvent(QPaintEvent* event) override;
	virtual void setModel(QAbstractItemModel* model) override;

	enum EmptyMode
	{
		Empty,
		String,
		ErrorString
	};

	void setEmptyString(QString emptyString);
	void setEmptyErrorString(QString emptyErrorString);
	void setEmptyMode(EmptyMode mode);

  public slots:
	virtual void reset() override;

  protected slots:
	virtual void rowsAboutToBeRemoved(const QModelIndex& parent, int start, int end) override;
	virtual void rowsInserted(const QModelIndex& parent, int start, int end) override;

  private: /* methods */
	void paintInfoLabel(QPaintEvent* event) const;
	void updateEmptyViewPort();
	QString currentEmptyString() const;

  private: /* variables */
	int m_itemCount = 0;
	QString m_emptyString;
	QString m_emptyErrorString;
	EmptyMode m_emptyMode = Empty;
};
