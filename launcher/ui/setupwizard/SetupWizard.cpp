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
#include "SetupWizard.h"

#include "JavaWizardPage.h"
#include "LanguageWizardPage.h"

#include <Application.h>
#include <FileSystem.h>
#include "translations/TranslationsModel.h"

#include <BuildConfig.h>
#include <QAbstractButton>

SetupWizard::SetupWizard(QWidget* parent) : QWizard(parent)
{
	setObjectName(QStringLiteral("SetupWizard"));
	resize(620, 660);
	setMinimumSize(300, 400);
	// make it ugly everywhere to avoid variability in theming
	setWizardStyle(QWizard::ClassicStyle);
	setOptions(QWizard::NoCancelButton | QWizard::IndependentPages | QWizard::HaveCustomButton1);

	retranslate();

	connect(this, &QWizard::currentIdChanged, this, &SetupWizard::pageChanged);
}

void SetupWizard::retranslate()
{
	setButtonText(QWizard::NextButton, tr("&Next >"));
	setButtonText(QWizard::BackButton, tr("< &Back"));
	setButtonText(QWizard::FinishButton, tr("&Finish"));
	setButtonText(QWizard::CustomButton1, tr("&Refresh"));
	setWindowTitle(tr("%1 Quick Setup").arg(BuildConfig.LAUNCHER_DISPLAYNAME));
}

BaseWizardPage* SetupWizard::getBasePage(int id)
{
	if (id == -1)
		return nullptr;
	auto pagePtr = page(id);
	if (!pagePtr)
		return nullptr;
	return dynamic_cast<BaseWizardPage*>(pagePtr);
}

BaseWizardPage* SetupWizard::getCurrentBasePage()
{
	return getBasePage(currentId());
}

void SetupWizard::pageChanged(int id)
{
	auto basePagePtr = getBasePage(id);
	if (!basePagePtr)
	{
		return;
	}
	if (basePagePtr->wantsRefreshButton())
	{
		setButtonLayout({ QWizard::CustomButton1,
						  QWizard::Stretch,
						  QWizard::BackButton,
						  QWizard::NextButton,
						  QWizard::FinishButton });
		auto customButton = button(QWizard::CustomButton1);
		connect(customButton,
				&QAbstractButton::clicked,
				[this]()
				{
					auto basePagePtr = getCurrentBasePage();
					if (basePagePtr)
					{
						basePagePtr->refresh();
					}
				});
	}
	else
	{
		setButtonLayout({ QWizard::Stretch, QWizard::BackButton, QWizard::NextButton, QWizard::FinishButton });
	}
}

void SetupWizard::changeEvent(QEvent* event)
{
	if (event->type() == QEvent::LanguageChange)
	{
		retranslate();
	}
	QWizard::changeEvent(event);
}

SetupWizard::~SetupWizard()
{}
