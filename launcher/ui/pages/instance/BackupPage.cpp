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

#include "BackupPage.h"
#include <QCheckBox>
#include <QInputDialog>
#include <QMessageBox>
#include "Application.h"
#include "ui_BackupDialog.h"

BackupPage::BackupPage(MinecraftInstance* inst, QWidget* parent)
	: QWidget(parent),
	  ui(new Ui::BackupDialog),
	  m_instance(inst),
	  m_backupManager(new BackupManager(this))
{
	ui->setupUi(this);
	setupConnections();
}

BackupPage::~BackupPage()
{
	delete ui;
}

QIcon BackupPage::icon() const
{
	return QIcon::fromTheme("screenshot-placeholder");
}

void BackupPage::retranslate()
{
	ui->retranslateUi(this);
}

void BackupPage::openedImpl()
{
	refreshBackupList();
	updateEstimatedSize();
}

void BackupPage::closedImpl()
{
	// Nothing to do
}

void BackupPage::updateEstimatedSize()
{
	BackupOptions options = getSelectedOptions();
	qint64 estimatedSize  = options.estimateSize();

	// Format size
	double sizeMB	 = estimatedSize / (1024.0 * 1024.0);
	QString sizeText = QString("Estimated Size: ~%1 MB").arg(sizeMB, 0, 'f', 1);

	ui->estimatedSizeLabel->setText(sizeText);
}

void BackupPage::setupConnections()
{
	// Connect checkboxes to update estimated size
	connect(ui->includeSaves, &QCheckBox::toggled, this, &BackupPage::updateEstimatedSize);
	connect(ui->includeConfig, &QCheckBox::toggled, this, &BackupPage::updateEstimatedSize);
	connect(ui->includeMods, &QCheckBox::toggled, this, &BackupPage::updateEstimatedSize);
	connect(ui->includeResourcePacks, &QCheckBox::toggled, this, &BackupPage::updateEstimatedSize);
	connect(ui->includeShaderPacks, &QCheckBox::toggled, this, &BackupPage::updateEstimatedSize);
	connect(ui->includeScreenshots, &QCheckBox::toggled, this, &BackupPage::updateEstimatedSize);
	connect(ui->includeOptions, &QCheckBox::toggled, this, &BackupPage::updateEstimatedSize);

	// Connect custom paths changes
	connect(ui->customPathsList, &QListWidget::itemChanged, this, &BackupPage::updateEstimatedSize);

	// Other connections
	connect(ui->refreshButton, &QPushButton::clicked, this, &BackupPage::refreshBackupList);
	connect(ui->backupList, &QListWidget::currentRowChanged, this, &BackupPage::updateBackupDetails);
}

void BackupPage::refreshBackupList()
{
	ui->backupList->clear();

	// Convert MinecraftInstance* to InstancePtr
	auto instancePtr = m_instance->shared_from_this();
	m_backups		 = m_backupManager->listBackups(instancePtr);

	for (const InstanceBackup& backup : m_backups)
	{
		QString displayText = QString("%1 - %2 (%3)")
								  .arg(backup.name())
								  .arg(backup.createdAt().toString("yyyy-MM-dd HH:mm"))
								  .arg(backup.displaySize());
		ui->backupList->addItem(displayText);
	}

	updateBackupDetails();
}

void BackupPage::updateBackupDetails()
{
	int currentRow = ui->backupList->currentRow();
	if (currentRow < 0 || currentRow >= m_backups.size())
	{
		ui->backupDetails->clear();
		ui->restoreButton->setEnabled(false);
		ui->deleteButton->setEnabled(false);
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
	ui->restoreButton->setEnabled(true);
	ui->deleteButton->setEnabled(true);
}

void BackupPage::on_createButton_clicked()
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

	auto instancePtr = m_instance->shared_from_this();
	m_backupManager->createBackupAsync(instancePtr, backupName, options);
}

void BackupPage::on_restoreButton_clicked()
{
	int currentRow = ui->backupList->currentRow();
	if (currentRow < 0 || currentRow >= m_backups.size())
	{
		return;
	}

	const InstanceBackup& backup = m_backups[currentRow];

	// Create custom dialog with checkbox
	QMessageBox msgBox(this);
	msgBox.setWindowTitle(tr("Restore Backup"));
	msgBox.setText(tr("Are you sure you want to restore backup '%1'?").arg(backup.name()));
	msgBox.setInformativeText(tr("This will overwrite current instance data."));
	msgBox.setIcon(QMessageBox::Question);

	QCheckBox* safetyCheckBox = new QCheckBox(tr("Create safety backup before restoring (recommended)"));
	safetyCheckBox->setChecked(true);
	msgBox.setCheckBox(safetyCheckBox);

	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msgBox.setDefaultButton(QMessageBox::No);

	if (msgBox.exec() != QMessageBox::Yes)
	{
		return;
	}

	bool createSafetyBackup = safetyCheckBox->isChecked();

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

	auto instancePtr = m_instance->shared_from_this();
	m_backupManager->restoreBackupAsync(instancePtr, backup, createSafetyBackup);
}

void BackupPage::on_deleteButton_clicked()
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

void BackupPage::on_backupList_currentRowChanged(int)
{
	updateBackupDetails();
}

BackupOptions BackupPage::getSelectedOptions() const
{
	BackupOptions options;
	options.includeSaves		 = ui->includeSaves->isChecked();
	options.includeConfig		 = ui->includeConfig->isChecked();
	options.includeMods			 = ui->includeMods->isChecked();
	options.includeResourcePacks = ui->includeResourcePacks->isChecked();
	options.includeShaderPacks	 = ui->includeShaderPacks->isChecked();
	options.includeScreenshots	 = ui->includeScreenshots->isChecked();
	options.includeOptions		 = ui->includeOptions->isChecked();

	// Get custom paths from list widget
	QStringList customPaths;
	for (int i = 0; i < ui->customPathsList->count(); ++i)
	{
		customPaths.append(ui->customPathsList->item(i)->text());
	}
	options.customPaths = customPaths;

	return options;
}

void BackupPage::on_addCustomPathButton_clicked()
{
	QString path = QInputDialog::getText(this,
										 tr("Add Custom Path"),
										 tr("Enter relative path to include (e.g., \"logs\", \"crash-reports\"):"),
										 QLineEdit::Normal,
										 QString(),
										 nullptr);

	if (!path.isEmpty())
	{
		// Check if already exists
		for (int i = 0; i < ui->customPathsList->count(); ++i)
		{
			if (ui->customPathsList->item(i)->text() == path)
			{
				return; // Already exists
			}
		}
		ui->customPathsList->addItem(path);
		updateEstimatedSize();
	}
}

void BackupPage::on_removeCustomPathButton_clicked()
{
	int currentRow = ui->customPathsList->currentRow();
	if (currentRow >= 0)
	{
		delete ui->customPathsList->takeItem(currentRow);
		updateEstimatedSize();
	}
}
