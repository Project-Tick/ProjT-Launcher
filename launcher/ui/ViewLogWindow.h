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

#include <QMainWindow>

#include "Application.h"

class OtherLogsPage;

class ViewLogWindow : public QMainWindow {
    Q_OBJECT

   public:
    explicit ViewLogWindow(QWidget* parent = nullptr);

   signals:
    void isClosing();

   protected:
    void closeEvent(QCloseEvent*) override;

   private:
    OtherLogsPage* m_page;
};
