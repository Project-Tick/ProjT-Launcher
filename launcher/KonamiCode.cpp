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
#include "KonamiCode.h"

#include <QDebug>
#include <array>

namespace
{
	const std::array<Qt::Key, 10> konamiCode = { { Qt::Key_Up,
												   Qt::Key_Up,
												   Qt::Key_Down,
												   Qt::Key_Down,
												   Qt::Key_Left,
												   Qt::Key_Right,
												   Qt::Key_Left,
												   Qt::Key_Right,
												   Qt::Key_B,
												   Qt::Key_A } };
}

KonamiCode::KonamiCode(QObject* parent) : QObject(parent)
{}

void KonamiCode::input(QEvent* event)
{
	if (event->type() == QEvent::KeyPress)
	{
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
		auto key			= Qt::Key(keyEvent->key());
		if (key == konamiCode[m_progress])
		{
			m_progress++;
		}
		else
		{
			m_progress = 0;
		}
		if (m_progress == static_cast<int>(konamiCode.size()))
		{
			m_progress = 0;
			emit triggered();
		}
	}
}
