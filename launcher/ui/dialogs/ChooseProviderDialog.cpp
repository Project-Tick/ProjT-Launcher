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
#include "ChooseProviderDialog.h"
#include "ui_ChooseProviderDialog.h"

#include <QPushButton>
#include <QRadioButton>

#include "modplatform/ModIndex.h"

ChooseProviderDialog::ChooseProviderDialog(QWidget* parent, bool single_choice, bool allow_skipping)
	: QDialog(parent),
	  ui(new Ui::ChooseProviderDialog)
{
	ui->setupUi(this);

	addProviders();
	m_providers.button(0)->click();

	connect(ui->skipOneButton, &QPushButton::clicked, this, &ChooseProviderDialog::skipOne);
	connect(ui->skipAllButton, &QPushButton::clicked, this, &ChooseProviderDialog::skipAll);

	connect(ui->confirmOneButton, &QPushButton::clicked, this, &ChooseProviderDialog::confirmOne);
	connect(ui->confirmAllButton, &QPushButton::clicked, this, &ChooseProviderDialog::confirmAll);

	if (single_choice)
	{
		ui->providersLayout->removeWidget(ui->skipAllButton);
		ui->providersLayout->removeWidget(ui->confirmAllButton);
	}

	if (!allow_skipping)
	{
		ui->providersLayout->removeWidget(ui->skipOneButton);
		ui->providersLayout->removeWidget(ui->skipAllButton);
	}
}

ChooseProviderDialog::~ChooseProviderDialog()
{
	delete ui;
}

void ChooseProviderDialog::setDescription(QString desc)
{
	ui->explanationLabel->setText(desc);
}

void ChooseProviderDialog::skipOne()
{
	reject();
}
void ChooseProviderDialog::skipAll()
{
	m_response.skip_all = true;
	reject();
}

void ChooseProviderDialog::confirmOne()
{
	m_response.chosen	  = getSelectedProvider();
	m_response.try_others = ui->tryOthersCheckbox->isChecked();
	accept();
}
void ChooseProviderDialog::confirmAll()
{
	m_response.chosen	   = getSelectedProvider();
	m_response.confirm_all = true;
	m_response.try_others  = ui->tryOthersCheckbox->isChecked();
	accept();
}

auto ChooseProviderDialog::getSelectedProvider() const -> ModPlatform::ResourceProvider
{
	return ModPlatform::ResourceProvider(m_providers.checkedId());
}

void ChooseProviderDialog::addProviders()
{
	int btn_index = 0;
	QRadioButton* btn;

	for (auto& provider : { ModPlatform::ResourceProvider::MODRINTH, ModPlatform::ResourceProvider::FLAME })
	{
		btn = new QRadioButton(ModPlatform::ProviderCapabilities::readableName(provider), this);
		m_providers.addButton(btn, btn_index++);
		ui->providersLayout->addWidget(btn);
	}
}

void ChooseProviderDialog::disableInput()
{
	for (auto& btn : m_providers.buttons())
		btn->setEnabled(false);

	ui->skipOneButton->setEnabled(false);
	ui->skipAllButton->setEnabled(false);
	ui->confirmOneButton->setEnabled(false);
	ui->confirmAllButton->setEnabled(false);
}
