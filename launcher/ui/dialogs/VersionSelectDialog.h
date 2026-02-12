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
 *
 *
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (C) 2023 Rachel Powers <508861+Ryex@users.noreply.github.com>
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
 * This file incorporates work covered by the following copyright and
 * permission notice:
 *
 *      Copyright 2013-2021 MultiMC Contributors
 *
 *      Licensed under the Apache License, Version 2.0 (the "License");
 *      you may not use this file except in compliance with the License.
 *      You may obtain a copy of the License at
 *
 *          http://www.apache.org/licenses/LICENSE-2.0
 *
 *      Unless required by applicable law or agreed to in writing, software
 *      distributed under the License is distributed on an "AS IS" BASIS,
 *      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *      See the License for the specific language governing permissions and
 *      limitations under the License.
 *
 * ======================================================================== */

#pragma once

#include <QDialog>

class VersionSelectWidget;
class QDialogButtonBox;
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class BaseVersionList;
#include "BaseVersion.h"
#include "BaseVersionList.h"

class VersionSelectDialog : public QDialog
{
	Q_OBJECT
  public:
	VersionSelectDialog(BaseVersionList* vlist, QString title, QWidget* parent = 0, bool cancelable = true);

	int exec() override;
	BaseVersion::Ptr selectedVersion() const;
	void setResizeOn(int column);
	void setEmptyString(QString emptyString);
	void setEmptyErrorString(QString emptyErrorString);
	void setCurrentVersion(const QString& version);
	void selectRecommended();

	void setExactFilter(BaseVersionList::ModelRoles role, QString filter);
	void setExactIfPresentFilter(BaseVersionList::ModelRoles role, QString filter);
	void setFuzzyFilter(BaseVersionList::ModelRoles role, QString filter);

  public slots:
	virtual void retranslate();

  private slots:
	void on_refreshButton_clicked();

  private:
	QVBoxLayout* m_verticalLayout;
	QHBoxLayout* m_horizontalLayout;
	VersionSelectWidget* m_versionWidget;
	QPushButton* m_refreshButton;
	QDialogButtonBox* m_buttonBox;
	BaseVersionList* m_vlist = nullptr;
	QString m_currentVersion;
	QString m_title; // Added member to store the title
	int resizeOnColumn = -1;
};
