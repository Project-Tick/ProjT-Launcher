// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  This file is part of ProjT Launcher and is licensed under
 *  the GNU General Public License version 3 or later.
 *
 */

#pragma once

#include <QWidget>
#include "minecraft/MinecraftInstance.h"
#include "minecraft/BackupManager.h"
#include "ui/pages/BasePage.h"

namespace Ui {
class BackupDialog;
}

class BackupPage : public QWidget, public BasePage {
    Q_OBJECT

public:
    explicit BackupPage(MinecraftInstance* inst, QWidget* parent = nullptr);
    ~BackupPage() override;

    QString displayName() const override { return tr("Backups"); }
    QIcon icon() const override;
    QString id() const override { return "backups"; }
    QString helpPage() const override { return "Backup-management"; }
    bool apply() override { return true; }

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

private:
    BackupOptions getSelectedOptions() const;
    void setupConnections();

    Ui::BackupDialog* ui;
    MinecraftInstance* m_instance;
    BackupManager* m_backupManager;
    QList<InstanceBackup> m_backups;
};
