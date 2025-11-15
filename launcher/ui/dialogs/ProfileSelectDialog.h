// SPDX-License-Identifier: GPL-3.0-or-later AND Apache-2.0
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

#include <memory>

#include "minecraft/auth/AccountList.h"

namespace Ui {
class ProfileSelectDialog;
}

class ProfileSelectDialog : public QDialog {
    Q_OBJECT
   public:
    enum Flags {
        NoFlags = 0,

        /*!
         * Shows a check box on the dialog that allows the user to specify that the account
         * they've selected should be used as the global default for all instances.
         */
        GlobalDefaultCheckbox,

        /*!
         * Shows a check box on the dialog that allows the user to specify that the account
         * they've selected should be used as the default for the instance they are currently launching.
         * This is not currently implemented.
         */
        InstanceDefaultCheckbox,
    };

    /*!
     * Constructs a new account select dialog with the given parent and message.
     * The message will be shown at the top of the dialog. It is an empty string by default.
     */
    explicit ProfileSelectDialog(const QString& message = "", int flags = 0, QWidget* parent = 0);
    ~ProfileSelectDialog();

    /*!
     * Gets a pointer to the account that the user selected.
     * This is null if the user clicked cancel or hasn't clicked OK yet.
     */
    MinecraftAccountPtr selectedAccount() const;

    /*!
     * Returns true if the user checked the "use as global default" checkbox.
     * If the checkbox wasn't shown, this function returns false.
     */
    bool useAsGlobalDefault() const;

    /*!
     * Returns true if the user checked the "use as instance default" checkbox.
     * If the checkbox wasn't shown, this function returns false.
     */
    bool useAsInstDefaullt() const;

   public slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

   protected:
    shared_qobject_ptr<AccountList> m_accounts;

    //! The account that was selected when the user clicked OK.
    MinecraftAccountPtr m_selected;

   private:
    Ui::ProfileSelectDialog* ui;
};
