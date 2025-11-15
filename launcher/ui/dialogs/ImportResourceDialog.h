// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  This file is part of ProjT Launcher and is licensed under
 *  the GNU General Public License version 3 or later.
 *
 *  If this file includes work from previous open-source projects,
 *  their original copyright and license notices are preserved below.
 */
#pragma once

#include <QDialog>
#include <QItemSelection>

#include "modplatform/ResourceType.h"
#include "ui/instanceview/InstanceProxyModel.h"

namespace Ui {
class ImportResourceDialog;
}

class ImportResourceDialog : public QDialog {
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
