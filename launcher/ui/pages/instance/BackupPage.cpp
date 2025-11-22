// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  This file is part of ProjT Launcher and is licensed under
 *  the GNU General Public License version 3 or later.
 */

#include "BackupPage.h"
#include "ui_BackupDialog.h"
#include <QMessageBox>
#include <QInputDialog>
#include "Application.h"

BackupPage::BackupPage(MinecraftInstance* inst, QWidget* parent)
    : QWidget(parent), ui(new Ui::BackupDialog), m_instance(inst), m_backupManager(new BackupManager(this))
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
    return APPLICATION->getThemedIcon("screenshot-placeholder");
}

void BackupPage::retranslate()
{
    ui->retranslateUi(this);
}

void BackupPage::openedImpl()
{
    refreshBackupList();
}

void BackupPage::closedImpl()
{
    // Nothing to do
}

void BackupPage::setupConnections()
{
    connect(ui->createButton, &QPushButton::clicked, this, &BackupPage::on_createButton_clicked);
    connect(ui->restoreButton, &QPushButton::clicked, this, &BackupPage::on_restoreButton_clicked);
    connect(ui->deleteButton, &QPushButton::clicked, this, &BackupPage::on_deleteButton_clicked);
    connect(ui->backupList, &QListWidget::currentRowChanged, this, &BackupPage::on_backupList_currentRowChanged);
    connect(ui->addCustomPathButton, &QPushButton::clicked, this, &BackupPage::on_addCustomPathButton_clicked);
    connect(ui->removeCustomPathButton, &QPushButton::clicked, this, &BackupPage::on_removeCustomPathButton_clicked);
}

void BackupPage::refreshBackupList()
{
    ui->backupList->clear();
    
    // Convert MinecraftInstance* to InstancePtr
    auto instancePtr = m_instance->shared_from_this();
    m_backups = m_backupManager->listBackups(instancePtr);
    
    for (const InstanceBackup& backup : m_backups) {
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
    if (currentRow < 0 || currentRow >= m_backups.size()) {
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
    
    if (!backup.description().isEmpty()) {
        details += tr("<b>Description:</b> %1<br>").arg(backup.description());
    }
    
    if (!backup.includedPaths().isEmpty()) {
        details += tr("<b>Included:</b> %1").arg(backup.includedPaths().join(", "));
    }
    
    ui->backupDetails->setHtml(details);
    ui->restoreButton->setEnabled(true);
    ui->deleteButton->setEnabled(true);
}

void BackupPage::on_createButton_clicked()
{
    bool ok;
    QString backupName = QInputDialog::getText(
        this,
        tr("Create Backup"),
        tr("Backup name:"),
        QLineEdit::Normal,
        QString(),
        &ok
    );
    
    if (!ok) {
        return;
    }
    
    BackupOptions options = getSelectedOptions();
    
    // Disable UI during backup
    ui->createButton->setEnabled(false);
    ui->restoreButton->setEnabled(false);
    ui->deleteButton->setEnabled(false);
    ui->createButton->setText(tr("Creating..."));
    
    // Connect signals for this operation
    connect(m_backupManager, &BackupManager::backupCreated, this, [this](const QString&, const QString&) {
        ui->createButton->setEnabled(true);
        ui->restoreButton->setEnabled(true);
        ui->deleteButton->setEnabled(true);
        ui->createButton->setText(tr("Create Backup"));
        QMessageBox::information(this, tr("Success"), tr("Backup created successfully!"));
        refreshBackupList();
        disconnect(m_backupManager, &BackupManager::backupCreated, this, nullptr);
        disconnect(m_backupManager, &BackupManager::backupFailed, this, nullptr);
    }, Qt::SingleShotConnection);
    
    connect(m_backupManager, &BackupManager::backupFailed, this, [this](const QString&, const QString& error) {
        ui->createButton->setEnabled(true);
        ui->restoreButton->setEnabled(true);
        ui->deleteButton->setEnabled(true);
        ui->createButton->setText(tr("Create Backup"));
        QMessageBox::critical(this, tr("Error"), tr("Failed to create backup: %1").arg(error));
        disconnect(m_backupManager, &BackupManager::backupCreated, this, nullptr);
        disconnect(m_backupManager, &BackupManager::backupFailed, this, nullptr);
    }, Qt::SingleShotConnection);
    
    auto instancePtr = m_instance->shared_from_this();
    m_backupManager->createBackupAsync(instancePtr, backupName, options);
}

void BackupPage::on_restoreButton_clicked()
{
    int currentRow = ui->backupList->currentRow();
    if (currentRow < 0 || currentRow >= m_backups.size()) {
        return;
    }
    
    const InstanceBackup& backup = m_backups[currentRow];
    
    auto result = QMessageBox::question(
        this,
        tr("Restore Backup"),
        tr("Are you sure you want to restore backup '%1'?\nThis will overwrite current instance data.").arg(backup.name()),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
    );
    
    if (result != QMessageBox::Yes) {
        return;
    }
    
    bool createSafetyBackup = QMessageBox::question(
        this,
        tr("Safety Backup"),
        tr("Create a safety backup before restoring?"),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::Yes
    ) == QMessageBox::Yes;
    
    // Disable UI during restore
    ui->createButton->setEnabled(false);
    ui->restoreButton->setEnabled(false);
    ui->deleteButton->setEnabled(false);
    ui->restoreButton->setText(tr("Restoring..."));
    
    // Connect signals for this operation
    connect(m_backupManager, &BackupManager::backupRestored, this, [this](const QString&, const QString&) {
        ui->createButton->setEnabled(true);
        ui->restoreButton->setEnabled(true);
        ui->deleteButton->setEnabled(true);
        ui->restoreButton->setText(tr("Restore"));
        QMessageBox::information(this, tr("Success"), tr("Backup restored successfully!"));
        refreshBackupList();
        disconnect(m_backupManager, &BackupManager::backupRestored, this, nullptr);
        disconnect(m_backupManager, &BackupManager::restoreFailed, this, nullptr);
    }, Qt::SingleShotConnection);
    
    connect(m_backupManager, &BackupManager::restoreFailed, this, [this](const QString&, const QString& error) {
        ui->createButton->setEnabled(true);
        ui->restoreButton->setEnabled(true);
        ui->deleteButton->setEnabled(true);
        ui->restoreButton->setText(tr("Restore"));
        QMessageBox::critical(this, tr("Error"), tr("Failed to restore backup: %1").arg(error));
        disconnect(m_backupManager, &BackupManager::backupRestored, this, nullptr);
        disconnect(m_backupManager, &BackupManager::restoreFailed, this, nullptr);
    }, Qt::SingleShotConnection);
    
    auto instancePtr = m_instance->shared_from_this();
    m_backupManager->restoreBackupAsync(instancePtr, backup, createSafetyBackup);
}

void BackupPage::on_deleteButton_clicked()
{
    int currentRow = ui->backupList->currentRow();
    if (currentRow < 0 || currentRow >= m_backups.size()) {
        return;
    }
    
    const InstanceBackup& backup = m_backups[currentRow];
    
    auto result = QMessageBox::question(
        this,
        tr("Delete Backup"),
        tr("Are you sure you want to delete backup '%1'?").arg(backup.name()),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
    );
    
    if (result != QMessageBox::Yes) {
        return;
    }
    
    if (m_backupManager->deleteBackup(backup)) {
        refreshBackupList();
        QMessageBox::information(this, tr("Success"), tr("Backup deleted successfully!"));
    } else {
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
    options.includeSaves = ui->includeSaves->isChecked();
    options.includeConfig = ui->includeConfig->isChecked();
    options.includeMods = ui->includeMods->isChecked();
    options.includeResourcePacks = ui->includeResourcePacks->isChecked();
    options.includeShaderPacks = ui->includeShaderPacks->isChecked();
    options.includeScreenshots = ui->includeScreenshots->isChecked();
    options.includeOptions = ui->includeOptions->isChecked();
    
    // Get custom paths from list widget
    QStringList customPaths;
    for (int i = 0; i < ui->customPathsList->count(); ++i) {
        customPaths.append(ui->customPathsList->item(i)->text());
    }
    options.customPaths = customPaths;
    
    return options;
}

void BackupPage::on_addCustomPathButton_clicked()
{
    QString path = QInputDialog::getText(
        this,
        tr("Add Custom Path"),
        tr("Enter relative path to include (e.g., \"logs\", \"crash-reports\"):"),
        QLineEdit::Normal,
        QString(),
        nullptr
    );
    
    if (!path.isEmpty()) {
        // Check if already exists
        for (int i = 0; i < ui->customPathsList->count(); ++i) {
            if (ui->customPathsList->item(i)->text() == path) {
                return;  // Already exists
            }
        }
        ui->customPathsList->addItem(path);
    }
}

void BackupPage::on_removeCustomPathButton_clicked()
{
    int currentRow = ui->customPathsList->currentRow();
    if (currentRow >= 0) {
        delete ui->customPathsList->takeItem(currentRow);
    }
}
