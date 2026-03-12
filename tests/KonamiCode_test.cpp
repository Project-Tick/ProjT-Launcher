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

#include <QEvent>
#include <QSignalSpy>
#include <QTest>

#include <KonamiCode.h>

class KonamiCodeTest : public QObject
{
	Q_OBJECT

	void feedKey(KonamiCode& konami, Qt::Key key)
	{
		QKeyEvent event(QEvent::KeyPress, key, Qt::NoModifier);
		konami.input(&event);
	}

  private slots:
	void test_doesNotTriggerOnNonKeyEvent()
	{
		KonamiCode konami;
		QSignalSpy spy(&konami, &KonamiCode::triggered);

		QEvent event(QEvent::MouseButtonPress);
		konami.input(&event);

		QCOMPARE(spy.count(), 0);
	}

	void test_triggersOnExactSequence()
	{
		KonamiCode konami;
		QSignalSpy spy(&konami, &KonamiCode::triggered);

		const QList<Qt::Key> sequence = { Qt::Key_Up, Qt::Key_Up,	 Qt::Key_Down, Qt::Key_Down, Qt::Key_Left,
										   Qt::Key_Right, Qt::Key_Left, Qt::Key_Right, Qt::Key_B,	Qt::Key_A };

		for (const auto key : sequence)
		{
			feedKey(konami, key);
		}

		QCOMPARE(spy.count(), 1);
	}

	void test_wrongKeyResetsProgress()
	{
		KonamiCode konami;
		QSignalSpy spy(&konami, &KonamiCode::triggered);

		feedKey(konami, Qt::Key_Up);
		feedKey(konami, Qt::Key_Up);
		feedKey(konami, Qt::Key_X); // wrong key resets progress

		const QList<Qt::Key> sequence = { Qt::Key_Up, Qt::Key_Up,	 Qt::Key_Down, Qt::Key_Down, Qt::Key_Left,
										   Qt::Key_Right, Qt::Key_Left, Qt::Key_Right, Qt::Key_B,	Qt::Key_A };
		for (const auto key : sequence)
		{
			feedKey(konami, key);
		}

		QCOMPARE(spy.count(), 1);
	}

	void test_canTriggerTwiceBackToBack()
	{
		KonamiCode konami;
		QSignalSpy spy(&konami, &KonamiCode::triggered);

		const QList<Qt::Key> sequence = { Qt::Key_Up, Qt::Key_Up,	 Qt::Key_Down, Qt::Key_Down, Qt::Key_Left,
										   Qt::Key_Right, Qt::Key_Left, Qt::Key_Right, Qt::Key_B,	Qt::Key_A };

		for (int i = 0; i < 2; ++i)
		{
			for (const auto key : sequence)
			{
				feedKey(konami, key);
			}
		}

		QCOMPARE(spy.count(), 2);
	}
};

QTEST_GUILESS_MAIN(KonamiCodeTest)

#include "KonamiCode_test.moc"
