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

#include <QDialog>
#include <QTreeWidgetItem>

namespace Ui
{
	class ReviewMessageBox;
}

class ReviewMessageBox : public QDialog
{
	Q_OBJECT

  public:
	static auto create(QWidget* parent, QString&& title, QString&& icon = "") -> ReviewMessageBox*;

	using ResourceInformation = struct res_info
	{
		QString name;
		QString filename;
		QString custom_file_path{};
		QString provider;
		QStringList required_by;
		QString version_type;
		bool enabled = true;
	};

	void appendResource(ResourceInformation&& info);
	auto deselectedResources() -> QStringList;

	void retranslateUi(QString resources_name);

	~ReviewMessageBox() override;

  protected slots:
	void on_toggleDepsButton_clicked();

  protected:
	ReviewMessageBox(QWidget* parent, const QString& title, const QString& icon);

	Ui::ReviewMessageBox* ui;

	QList<QTreeWidgetItem*> m_deps;
	bool m_deps_checked = true;
};
