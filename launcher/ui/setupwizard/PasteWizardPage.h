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
#ifndef PASTEDEFAULTSCONFIRMATIONWIZARD_H
#define PASTEDEFAULTSCONFIRMATIONWIZARD_H

#include <QWidget>
#include "BaseWizardPage.h"

namespace Ui {
class PasteWizardPage;
}

class PasteWizardPage : public BaseWizardPage {
    Q_OBJECT

   public:
    explicit PasteWizardPage(QWidget* parent = nullptr);
    ~PasteWizardPage();

    void initializePage() override;
    bool validatePage() override;
    void retranslate() override;

   private:
    Ui::PasteWizardPage* ui;
};

#endif  // PASTEDEFAULTSCONFIRMATIONWIZARD_H
