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
#include "ScrollMessageBox.h"
#include <QPushButton>
#include "ui_ScrollMessageBox.h"

ScrollMessageBox::ScrollMessageBox(QWidget* parent, const QString& title, const QString& text, const QString& body)
    : QDialog(parent), ui(new Ui::ScrollMessageBox)
{
    ui->setupUi(this);
    this->setWindowTitle(title);
    ui->label->setText(text);
    ui->textBrowser->setText(body);

    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
}

ScrollMessageBox::~ScrollMessageBox()
{
    delete ui;
}
