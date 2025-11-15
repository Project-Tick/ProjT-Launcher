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

QT_BEGIN_NAMESPACE
namespace Ui {
class ScrollMessageBox;
}
QT_END_NAMESPACE

class ScrollMessageBox : public QDialog {
    Q_OBJECT

   public:
    ScrollMessageBox(QWidget* parent, const QString& title, const QString& text, const QString& body);

    ~ScrollMessageBox() override;

   private:
    Ui::ScrollMessageBox* ui;
};
