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
#include "AutoJavaWizardPage.h"
#include "ui_AutoJavaWizardPage.h"

#include "Application.h"

AutoJavaWizardPage::AutoJavaWizardPage(QWidget* parent) : BaseWizardPage(parent), ui(new Ui::AutoJavaWizardPage)
{
    ui->setupUi(this);
}

AutoJavaWizardPage::~AutoJavaWizardPage()
{
    delete ui;
}

void AutoJavaWizardPage::initializePage() {}

bool AutoJavaWizardPage::validatePage()
{
    auto s = APPLICATION->settings();

    if (!ui->previousSettingsRadioButton->isChecked()) {
        s->set("AutomaticJavaSwitch", true);
        s->set("AutomaticJavaDownload", true);
    }
    s->set("UserAskedAboutAutomaticJavaDownload", true);
    return true;
}

void AutoJavaWizardPage::retranslate()
{
    ui->retranslateUi(this);
}
