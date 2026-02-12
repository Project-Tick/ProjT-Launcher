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

#pragma once

#include <QObjectPtr.h>
#include <QDialog>
#include <QStringListModel>
#include "JavaCommon.h"
#include "ui/pages/BasePage.h"
#include "ui/widgets/JavaSettingsWidget.h"

class SettingsObject;

namespace Ui
{
	class JavaPage;
}

class JavaPage : public QWidget, public BasePage
{
	Q_OBJECT

  public:
	explicit JavaPage(QWidget* parent = 0);
	~JavaPage();

	QString displayName() const override
	{
		return tr("Java");
	}
	QIcon icon() const override
	{
		return QIcon::fromTheme("java");
	}
	QString id() const override
	{
		return "java-settings";
	}
	QString helpPage() const override
	{
		return "Java-settings";
	}
	void retranslate() override;

	bool apply() override;

  private slots:
	void on_downloadJavaButton_clicked();
	void on_removeJavaButton_clicked();
	void on_refreshJavaButton_clicked();

  private:
	Ui::JavaPage* ui;
};
