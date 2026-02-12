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
 *
 * === Upstream License Block (Do Not Modify) ==============================
 *
 *
 *
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (C) 2025 TheKodeToad <TheKodeToad@proton.me>
 *  Copyright (C) 2022 Tayou <git@tayou.org>
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
 *
 * ======================================================================== */

#pragma once

#include <QDialog>
#include <memory>

#include <Application.h>
#include <translations/TranslationsModel.h>
#include <QTextCursor>
#include "ui/pages/BasePage.h"

class QTextCharFormat;
class SettingsObject;

namespace Ui
{
	class AppearanceWidget;
}

class AppearanceWidget : public QWidget
{
	Q_OBJECT

  public:
	explicit AppearanceWidget(bool simple, QWidget* parent = 0);
	virtual ~AppearanceWidget();

  public:
	void applySettings();
	void loadSettings();
	void retranslateUi();

  private:
	void applyIconTheme(int index);
	void applyWidgetTheme(int index);
	void applyCatTheme(int index);
	void loadThemeSettings();

	void updateConsolePreview();
	void updateCatPreview();

	Ui::AppearanceWidget* m_ui;
	QTextCharFormat m_defaultFormat;
	bool m_themesOnly;
};
