// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2026 Project Tick
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef PASTEDEFAULTSCONFIRMATIONWIZARD_H
#define PASTEDEFAULTSCONFIRMATIONWIZARD_H

#include <QWidget>
#include "BaseWizardPage.h"

namespace Ui
{
	class PasteWizardPage;
}

class PasteWizardPage : public BaseWizardPage
{
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

#endif // PASTEDEFAULTSCONFIRMATIONWIZARD_H
