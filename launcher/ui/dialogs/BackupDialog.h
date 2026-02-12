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

#include <QDialog>
#include "BaseInstance.h"
#include "minecraft/BackupManager.h"

namespace Ui
{
	class BackupDialog;
}

class BackupDialog : public QDialog
{
	Q_OBJECT

  public:
	explicit BackupDialog(InstancePtr instance, QWidget* parent = nullptr);
	~BackupDialog();

  private slots:
	void on_createButton_clicked();
	void on_restoreButton_clicked();
	void on_deleteButton_clicked();
	void on_refreshButton_clicked();
	void on_backupList_currentRowChanged(int currentRow);
	void on_addCustomPathButton_clicked();
	void on_removeCustomPathButton_clicked();
	void onBackupCreated(const QString& instanceId, const QString& backupName);
	void onBackupRestored(const QString& instanceId, const QString& backupName);

  private:
	void refreshBackupList();
	void updateBackupDetails();
	void updateButtons();
	BackupOptions getSelectedOptions() const;

	Ui::BackupDialog* ui;
	InstancePtr m_instance;
	BackupManager* m_backupManager;
	QList<InstanceBackup> m_backups;
	QStringList m_customPaths;
};
