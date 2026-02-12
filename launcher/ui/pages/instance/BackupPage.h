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
 */

#pragma once

#include <QWidget>
#include "minecraft/BackupManager.h"
#include "minecraft/MinecraftInstance.h"
#include "ui/pages/BasePage.h"

namespace Ui
{
	class BackupDialog;
}

class BackupPage : public QWidget, public BasePage
{
	Q_OBJECT

  public:
	explicit BackupPage(MinecraftInstance* inst, QWidget* parent = nullptr);
	~BackupPage() override;

	QString displayName() const override
	{
		return tr("Backups");
	}
	QIcon icon() const override;
	QString id() const override
	{
		return "backups";
	}
	QString helpPage() const override
	{
		return "Backup-management";
	}
	bool apply() override
	{
		return true;
	}

	void retranslate() override;
	void openedImpl() override;
	void closedImpl() override;

  private slots:
	void on_createButton_clicked();
	void on_restoreButton_clicked();
	void on_deleteButton_clicked();
	void on_backupList_currentRowChanged(int currentRow);
	void on_addCustomPathButton_clicked();
	void on_removeCustomPathButton_clicked();

	void refreshBackupList();
	void updateBackupDetails();
	void updateEstimatedSize();

  private:
	BackupOptions getSelectedOptions() const;
	void setupConnections();

	Ui::BackupDialog* ui;
	MinecraftInstance* m_instance;
	BackupManager* m_backupManager;
	QList<InstanceBackup> m_backups;
};
