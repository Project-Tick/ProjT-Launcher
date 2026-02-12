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

#include <QtWidgets/QDialog>

#include "minecraft/auth/MinecraftAccount.hpp"
#include "tasks/Task.h"

namespace Ui
{
	class OfflineLoginDialog;
}

class OfflineLoginDialog : public QDialog
{
	Q_OBJECT

  public:
	~OfflineLoginDialog();

	static MinecraftAccountPtr newAccount(QWidget* parent, QString message);

  private:
	explicit OfflineLoginDialog(QWidget* parent = 0);

	void setUserInputsEnabled(bool enable);

  protected slots:
	void accept();

	void onTaskFailed(const QString& reason);
	void onTaskSucceeded();
	void onTaskStatus(const QString& status);
	void onTaskProgress(qint64 current, qint64 total);

	void on_userTextBox_textEdited(const QString& newText);
	void on_allowLongUsernames_stateChanged(int value);

  private:
	Ui::OfflineLoginDialog* ui;
	MinecraftAccountPtr m_account;
	Task::Ptr m_loginTask;
};
