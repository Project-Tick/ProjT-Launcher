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

#pragma once

#include <QDialog>
#include "minecraft/BackupManager.h"
#include "BaseInstance.h"

namespace Ui {
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
};
