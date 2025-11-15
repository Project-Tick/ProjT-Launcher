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

#include <QKeyEvent>

class KonamiCode : public QObject {
    Q_OBJECT
   public:
    KonamiCode(QObject* parent = 0);
    void input(QEvent* event);

   signals:
    void triggered();

   private:
    int m_progress = 0;
};
