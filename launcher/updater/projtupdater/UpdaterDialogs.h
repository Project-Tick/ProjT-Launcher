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
 *
 *
 *
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (C) 2023 Rachel Powers <508861+Ryex@users.noreply.github.com>
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
 *
 *
 * ======================================================================== */

#pragma once

#include <QDialog>
#include <QTreeWidgetItem>

#include "ReleaseInfo.h"
#include "Version.h"

namespace Ui
{
	class SelectReleaseDialog;
}

class SelectReleaseDialog : public QDialog
{
	Q_OBJECT

  public:
	explicit SelectReleaseDialog(const Version& cur_version, const QList<ReleaseInfo>& releases, QWidget* parent = 0);
	~SelectReleaseDialog();

	void loadReleases();
	void appendRelease(ReleaseInfo const& release);
	ReleaseInfo selectedRelease()
	{
		return m_selectedRelease;
	}
  private slots:
	ReleaseInfo getRelease(QTreeWidgetItem* item);
	void selectionChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);

  protected:
	QList<ReleaseInfo> m_releases;
	ReleaseInfo m_selectedRelease;
	Version m_currentVersion;

	Ui::SelectReleaseDialog* ui;
};

class SelectReleaseAssetDialog : public QDialog
{
	Q_OBJECT
  public:
	explicit SelectReleaseAssetDialog(const QList<ReleaseAsset>& assets, QWidget* parent = 0);
	~SelectReleaseAssetDialog();

	void loadAssets();
	void appendAsset(ReleaseAsset const& asset);
	ReleaseAsset selectedAsset()
	{
		return m_selectedAsset;
	}
  private slots:
	ReleaseAsset getAsset(QTreeWidgetItem* item);
	void selectionChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);

  protected:
	QList<ReleaseAsset> m_assets;
	ReleaseAsset m_selectedAsset;

	Ui::SelectReleaseDialog* ui;
};
