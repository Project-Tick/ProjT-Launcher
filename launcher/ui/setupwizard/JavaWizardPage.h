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

#include "BaseWizardPage.h"

class JavaWizardWidget;

class JavaWizardPage : public BaseWizardPage {
    Q_OBJECT
   public:
    explicit JavaWizardPage(QWidget* parent = Q_NULLPTR);

    virtual ~JavaWizardPage() = default;

    bool wantsRefreshButton() override;
    void refresh() override;
    void initializePage() override;
    bool validatePage() override;

   protected: /* methods */
    void setupUi();
    void retranslate() override;

   private: /* data */
    JavaWizardWidget* m_java_widget = nullptr;
};
