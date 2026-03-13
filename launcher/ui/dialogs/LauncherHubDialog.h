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
#include <QString>
#include <QUrl>

class LauncherHubWidget;

class LauncherHubDialog : public QDialog
{
	Q_OBJECT

  public:
	explicit LauncherHubDialog(QWidget* parent = nullptr);
	~LauncherHubDialog() override;

	void openUrl(const QUrl& url);
	void setSelectedInstanceId(const QString& id);
	void refreshCockpit();

  signals:
	void selectInstanceRequested(const QString& instanceId);
	void launchInstanceRequested(const QString& instanceId);
	void editInstanceRequested(const QString& instanceId);
	void backupsRequested(const QString& instanceId);
	void openInstanceFolderRequested(const QString& instanceId);

  private:
	LauncherHubWidget* m_widget = nullptr;
};
