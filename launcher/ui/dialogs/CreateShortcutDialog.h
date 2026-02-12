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

#include <QDialog>
#include "BaseInstance.h"

class BaseInstance;

namespace Ui
{
	class CreateShortcutDialog;
}

class CreateShortcutDialog : public QDialog
{
	Q_OBJECT

  public:
	explicit CreateShortcutDialog(InstancePtr instance, QWidget* parent = nullptr);
	~CreateShortcutDialog();

	void createShortcut();

  private slots:
	// Icon, target and name
	void on_iconButton_clicked();

	// Override account
	void on_overrideAccountCheckbox_stateChanged(int state);

	// Override target (world, server)
	void on_targetCheckbox_stateChanged(int state);
	void on_worldTarget_toggled(bool checked);
	void on_serverTarget_toggled(bool checked);
	void on_worldSelectionBox_currentIndexChanged(int index);
	void on_serverAddressBox_textChanged(const QString& text);

  private:
	// Data
	Ui::CreateShortcutDialog* ui;
	QString InstIconKey;
	InstancePtr m_instance;
	bool m_QuickJoinSupported = false;

	// Functions
	void stateChanged();
};
