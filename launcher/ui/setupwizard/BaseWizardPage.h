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

#include <QEvent>
#include <QWizardPage>

class BaseWizardPage : public QWizardPage {
   public:
    explicit BaseWizardPage(QWidget* parent = Q_NULLPTR) : QWizardPage(parent) {}
    virtual ~BaseWizardPage() {};

    virtual bool wantsRefreshButton() { return false; }
    virtual void refresh() {}

   protected:
    virtual void retranslate() = 0;
    void changeEvent(QEvent* event) override
    {
        if (event->type() == QEvent::LanguageChange) {
            retranslate();
        }
        QWizardPage::changeEvent(event);
    }
};
