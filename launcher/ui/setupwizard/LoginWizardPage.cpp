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
#include "LoginWizardPage.h"
#include "minecraft/auth/AccountList.hpp"
#include "ui/dialogs/MSALoginDialog.h"
#include "ui_LoginWizardPage.h"

#include "Application.h"

LoginWizardPage::LoginWizardPage(QWidget* parent) : BaseWizardPage(parent), ui(new Ui::LoginWizardPage)
{
	ui->setupUi(this);
}

LoginWizardPage::~LoginWizardPage()
{
	delete ui;
}

void LoginWizardPage::initializePage()
{}

bool LoginWizardPage::validatePage()
{
	return true;
}

void LoginWizardPage::retranslate()
{
	ui->retranslateUi(this);
}

void LoginWizardPage::on_pushButton_clicked()
{
	wizard()->hide();
	auto account = MSALoginDialog::newAccount(nullptr);
	wizard()->show();
	if (account)
	{
		APPLICATION->accounts()->addAccount(account);
		APPLICATION->accounts()->setDefaultAccount(account);
		if (wizard()->currentId() == wizard()->pageIds().last())
		{
			wizard()->accept();
		}
		else
		{
			wizard()->next();
		}
	}
}
