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
#include <QItemSelection>

#include "modplatform/ResourceType.h"
#include "ui/instanceview/InstanceProxyModel.h"

namespace Ui
{
	class ImportResourceDialog;
}

class ImportResourceDialog : public QDialog
{
	Q_OBJECT

  public:
	explicit ImportResourceDialog(QString file_path, ModPlatform::ResourceType type, QWidget* parent = nullptr);
	~ImportResourceDialog() override;
	QString selectedInstanceKey;

  private:
	Ui::ImportResourceDialog* ui;
	ModPlatform::ResourceType m_resource_type;
	QString m_file_path;
	InstanceProxyModel* proxyModel;

  private slots:
	void selectionChanged(QItemSelection, QItemSelection);
	void activated(QModelIndex);
};
