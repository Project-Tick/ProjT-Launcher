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
#include "LanguageWizardPage.h"
#include <Application.h>
#include <translations/TranslationsModel.h>

#include <BuildConfig.h>
#include <QVBoxLayout>
#include "ui/widgets/LanguageSelectionWidget.h"

LanguageWizardPage::LanguageWizardPage(QWidget* parent) : BaseWizardPage(parent)
{
    setObjectName(QStringLiteral("languagePage"));
    auto layout = new QVBoxLayout(this);
    mainWidget = new LanguageSelectionWidget(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(mainWidget);

    retranslate();
}

LanguageWizardPage::~LanguageWizardPage() {}

bool LanguageWizardPage::wantsRefreshButton()
{
    return true;
}

void LanguageWizardPage::refresh()
{
    auto translations = APPLICATION->translations();
    translations->downloadIndex();
}

bool LanguageWizardPage::validatePage()
{
    auto settings = APPLICATION->settings();
    QString key = mainWidget->getSelectedLanguageKey();
    settings->set("Language", key);
    return true;
}

void LanguageWizardPage::retranslate()
{
    setTitle(tr("Language"));
    setSubTitle(tr("Select the language to use in %1").arg(BuildConfig.LAUNCHER_DISPLAYNAME));
    mainWidget->retranslate();
}
