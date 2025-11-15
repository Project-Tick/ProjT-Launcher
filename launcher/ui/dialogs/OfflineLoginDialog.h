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
 *  If this file includes work from previous open-source projects,
 *  their original copyright and license notices are preserved below.
 */
#pragma once

#include <QtWidgets/QDialog>

#include "minecraft/auth/MinecraftAccount.h"
#include "tasks/Task.h"

namespace Ui {
class OfflineLoginDialog;
}

class OfflineLoginDialog : public QDialog {
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
