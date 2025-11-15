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
 *
 * === Upstream License Block (Do Not Modify) ==============================
 *
 * // SPDX-License-Identifier: GPL-3.0-only
 *
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (c) 2023 Trial97 <alexandru.tripon97@gmail.com>
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
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * ======================================================================== */

#pragma once

#include <QDialog>
#include <QList>
#include "minecraft/mod/Mod.h"
#include "modplatform/helpers/ExportToModList.h"

namespace Ui {
class ExportToModListDialog;
}

class ExportToModListDialog : public QDialog {
    Q_OBJECT

   public:
    explicit ExportToModListDialog(QString name, QList<Mod*> mods, QWidget* parent = nullptr);
    ~ExportToModListDialog();

    void done(int result) override;

   protected slots:
    void formatChanged(int index);
    void triggerImp();
    void trigger(int) { triggerImp(); };
    void addExtra(ExportToModList::OptionalData option);

   private:
    QString extension();
    void enableCustom(bool enabled);

    QList<Mod*> m_mods;
    bool m_template_changed;
    QString m_name;
    ExportToModList::Formats m_format = ExportToModList::Formats::HTML;
    Ui::ExportToModListDialog* ui;
    static const QHash<ExportToModList::Formats, QString> exampleLines;
};
