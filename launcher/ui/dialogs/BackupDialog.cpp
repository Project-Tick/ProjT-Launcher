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

#include "BackupDialog.h"
#include <QInputDialog>
#include <QMessageBox>
#include "ui_BackupDialog.h"

BackupDialog::BackupDialog(InstancePtr instance, QWidget* parent)
	: QDialog(parent),
	  ui(new Ui::BackupDialog),
	  m_instance(instance),
	  m_backupManager(new BackupManager(this))
{
	ui->setupUi(this);

	setWindowTitle(tr("Manage Backups - %1").arg(instance->name()));

	// Connect signals
	connect(m_backupManager, &BackupManager::backupCreated, this, &BackupDialog::onBackupCreated);
	connect(m_backupManager, &BackupManager::backupRestored, this, &BackupDialog::onBackupRestored);

	// Load backups
	refreshBackupList();
}

BackupDialog::~BackupDialog()
{
	delete ui;
}

void BackupDialog::refreshBackupList()
{
	ui->backupList->clear();
	m_backups = m_backupManager->listBackups(m_instance);

	for (const InstanceBackup& backup : m_backups)
	{
		QString displayText = QString("%1 - %2 (%3)")
								  .arg(backup.name())
								  .arg(backup.createdAt().toString("yyyy-MM-dd HH:mm"))
								  .arg(backup.displaySize());
		ui->backupList->addItem(displayText);
	}

	updateButtons();
}

void BackupDialog::updateBackupDetails()
{
	int currentRow = ui->backupList->currentRow();
	if (currentRow < 0 || currentRow >= m_backups.size())
	{
		ui->backupDetails->clear();
		return;
	}

	const InstanceBackup& backup = m_backups[currentRow];

	QString details;
	details += tr("<b>Name:</b> %1<br>").arg(backup.name());
	details += tr("<b>Created:</b> %1<br>").arg(backup.createdAt().toString("yyyy-MM-dd HH:mm:ss"));
	details += tr("<b>Size:</b> %1<br>").arg(backup.displaySize());

	if (!backup.description().isEmpty())
	{
		details += tr("<b>Description:</b> %1<br>").arg(backup.description());
	}

	if (!backup.includedPaths().isEmpty())
	{
		details += tr("<b>Included:</b> %1").arg(backup.includedPaths().join(", "));
	}

	ui->backupDetails->setHtml(details);
}

void BackupDialog::updateButtons()
{
	bool hasSelection = ui->backupList->currentRow() >= 0;
	ui->restoreButton->setEnabled(hasSelection);
	ui->deleteButton->setEnabled(hasSelection);
}

void BackupDialog::on_createButton_clicked()
{
	bool ok;
	QString backupName =
		QInputDialog::getText(this, tr("Create Backup"), tr("Backup name:"), QLineEdit::Normal, QString(), &ok);

	if (!ok)
	{
		return;
	}

	BackupOptions options = getSelectedOptions();

	// Disable UI during backup
	ui->createButton->setEnabled(false);
	ui->restoreButton->setEnabled(false);
	ui->deleteButton->setEnabled(false);
	ui->createButton->setText(tr("Creating..."));

	// Connect signals for this operation
	connect(
		m_backupManager,
		&BackupManager::backupCreated,
		this,
		[this](const QString&, const QString&)
		{
			ui->createButton->setEnabled(true);
			ui->restoreButton->setEnabled(true);
			ui->deleteButton->setEnabled(true);
			ui->createButton->setText(tr("Create Backup"));
			QMessageBox::information(this, tr("Success"), tr("Backup created successfully!"));
			refreshBackupList();
			disconnect(m_backupManager, &BackupManager::backupCreated, this, nullptr);
			disconnect(m_backupManager, &BackupManager::backupFailed, this, nullptr);
		},
		Qt::SingleShotConnection);

	connect(
		m_backupManager,
		&BackupManager::backupFailed,
		this,
		[this](const QString&, const QString& error)
		{
			ui->createButton->setEnabled(true);
			ui->restoreButton->setEnabled(true);
			ui->deleteButton->setEnabled(true);
			ui->createButton->setText(tr("Create Backup"));
			QMessageBox::critical(this, tr("Error"), tr("Failed to create backup: %1").arg(error));
			disconnect(m_backupManager, &BackupManager::backupCreated, this, nullptr);
			disconnect(m_backupManager, &BackupManager::backupFailed, this, nullptr);
		},
		Qt::SingleShotConnection);

	m_backupManager->createBackupAsync(m_instance, backupName, options);
}

void BackupDialog::on_restoreButton_clicked()
{
	int currentRow = ui->backupList->currentRow();
	if (currentRow < 0 || currentRow >= m_backups.size())
	{
		return;
	}

	const InstanceBackup& backup = m_backups[currentRow];

	auto result = QMessageBox::question(
		this,
		tr("Restore Backup"),
		tr("Are you sure you want to restore backup '%1'?\nThis will overwrite current instance data.")
			.arg(backup.name()),
		QMessageBox::Yes | QMessageBox::No,
		QMessageBox::No);

	if (result != QMessageBox::Yes)
	{
		return;
	}

	bool createSafetyBackup = QMessageBox::question(this,
													tr("Safety Backup"),
													tr("Create a safety backup before restoring?"),
													QMessageBox::Yes | QMessageBox::No,
													QMessageBox::Yes)
						   == QMessageBox::Yes;

	// Disable UI during restore
	ui->createButton->setEnabled(false);
	ui->restoreButton->setEnabled(false);
	ui->deleteButton->setEnabled(false);
	ui->restoreButton->setText(tr("Restoring..."));

	// Connect signals for this operation
	connect(
		m_backupManager,
		&BackupManager::backupRestored,
		this,
		[this](const QString&, const QString&)
		{
			ui->createButton->setEnabled(true);
			ui->restoreButton->setEnabled(true);
			ui->deleteButton->setEnabled(true);
			ui->restoreButton->setText(tr("Restore"));
			QMessageBox::information(this, tr("Success"), tr("Backup restored successfully!"));
			refreshBackupList();
			disconnect(m_backupManager, &BackupManager::backupRestored, this, nullptr);
			disconnect(m_backupManager, &BackupManager::restoreFailed, this, nullptr);
		},
		Qt::SingleShotConnection);

	connect(
		m_backupManager,
		&BackupManager::restoreFailed,
		this,
		[this](const QString&, const QString& error)
		{
			ui->createButton->setEnabled(true);
			ui->restoreButton->setEnabled(true);
			ui->deleteButton->setEnabled(true);
			ui->restoreButton->setText(tr("Restore"));
			QMessageBox::critical(this, tr("Error"), tr("Failed to restore backup: %1").arg(error));
			disconnect(m_backupManager, &BackupManager::backupRestored, this, nullptr);
			disconnect(m_backupManager, &BackupManager::restoreFailed, this, nullptr);
		},
		Qt::SingleShotConnection);

	m_backupManager->restoreBackupAsync(m_instance, backup, createSafetyBackup);
}

void BackupDialog::on_deleteButton_clicked()
{
	int currentRow = ui->backupList->currentRow();
	if (currentRow < 0 || currentRow >= m_backups.size())
	{
		return;
	}

	const InstanceBackup& backup = m_backups[currentRow];

	auto result = QMessageBox::question(this,
										tr("Delete Backup"),
										tr("Are you sure you want to delete backup '%1'?").arg(backup.name()),
										QMessageBox::Yes | QMessageBox::No,
										QMessageBox::No);

	if (result != QMessageBox::Yes)
	{
		return;
	}

	if (m_backupManager->deleteBackup(backup))
	{
		refreshBackupList();
		QMessageBox::information(this, tr("Success"), tr("Backup deleted successfully!"));
	}
	else
	{
		QMessageBox::critical(this, tr("Error"), tr("Failed to delete backup."));
	}
}

void BackupDialog::on_refreshButton_clicked()
{
	refreshBackupList();
}

void BackupDialog::on_backupList_currentRowChanged(int)
{
	updateBackupDetails();
	updateButtons();
}

void BackupDialog::onBackupCreated(const QString& instanceId, const QString& backupName)
{
	if (instanceId == m_instance->id())
	{
		refreshBackupList();
	}
}

void BackupDialog::onBackupRestored(const QString& instanceId, const QString& backupName)
{
	if (instanceId == m_instance->id())
	{
		refreshBackupList();
	}
}

BackupOptions BackupDialog::getSelectedOptions() const
{
	BackupOptions options;
	options.includeSaves		 = ui->includeSaves->isChecked();
	options.includeConfig		 = ui->includeConfig->isChecked();
	options.includeMods			 = ui->includeMods->isChecked();
	options.includeResourcePacks = ui->includeResourcePacks->isChecked();
	options.includeShaderPacks	 = ui->includeShaderPacks->isChecked();
	options.includeScreenshots	 = ui->includeScreenshots->isChecked();
	options.includeOptions		 = ui->includeOptions->isChecked();
	options.customPaths			 = m_customPaths;
	return options;
}

void BackupDialog::on_addCustomPathButton_clicked()
{
	QString path = QInputDialog::getText(this,
										 tr("Add Custom Path"),
										 tr("Enter relative path to include (e.g., \"logs\", \"crash-reports\"):"),
										 QLineEdit::Normal,
										 QString(),
										 nullptr);

	if (!path.isEmpty() && !m_customPaths.contains(path))
	{
		m_customPaths.append(path);
		ui->customPathsList->addItem(path);
	}
}

void BackupDialog::on_removeCustomPathButton_clicked()
{
	int currentRow = ui->customPathsList->currentRow();
	if (currentRow >= 0 && currentRow < m_customPaths.size())
	{
		m_customPaths.removeAt(currentRow);
		delete ui->customPathsList->takeItem(currentRow);
	}
}
