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
#include "PasteWizardPage.h"
#include "ui_PasteWizardPage.h"

#include "Application.h"
#include "net/PasteUpload.h"

PasteWizardPage::PasteWizardPage(QWidget* parent) : BaseWizardPage(parent), ui(new Ui::PasteWizardPage)
{
    ui->setupUi(this);
}

PasteWizardPage::~PasteWizardPage()
{
    delete ui;
}

void PasteWizardPage::initializePage() {}

bool PasteWizardPage::validatePage()
{
    auto s = APPLICATION->settings();
    QString prevPasteURL = s->get("PastebinURL").toString();
    s->reset("PastebinURL");
    if (ui->previousSettingsRadioButton->isChecked()) {
        bool usingDefaultBase = prevPasteURL == PasteUpload::PasteTypes.at(PasteUpload::PasteType::NullPointer).defaultBase;
        s->set("PastebinType", PasteUpload::PasteType::NullPointer);
        if (!usingDefaultBase)
            s->set("PastebinCustomAPIBase", prevPasteURL);
    }

    return true;
}

void PasteWizardPage::retranslate()
{
    ui->retranslateUi(this);
}
