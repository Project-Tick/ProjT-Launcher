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
#include "KonamiCode.h"

#include <QDebug>
#include <array>

namespace {
const std::array<Qt::Key, 10> konamiCode = { { Qt::Key_Up, Qt::Key_Up, Qt::Key_Down, Qt::Key_Down, Qt::Key_Left, Qt::Key_Right,
                                               Qt::Key_Left, Qt::Key_Right, Qt::Key_B, Qt::Key_A } };
}

KonamiCode::KonamiCode(QObject* parent) : QObject(parent) {}

void KonamiCode::input(QEvent* event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        auto key = Qt::Key(keyEvent->key());
        if (key == konamiCode[m_progress]) {
            m_progress++;
        } else {
            m_progress = 0;
        }
        if (m_progress == static_cast<int>(konamiCode.size())) {
            m_progress = 0;
            emit triggered();
        }
    }
}
