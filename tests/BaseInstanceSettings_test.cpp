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

#include <QTemporaryDir>
#include <QTest>

#include <BaseInstance.h>
#include <settings/INISettingsObject.h>

class BaseInstanceSettingsTest : public QObject
{
	Q_OBJECT

  private slots:
	void test_consoleSettings()
	{
		QTemporaryDir dir;
		QVERIFY(dir.isValid());
		auto settings = std::make_shared<INISettingsObject>(dir.filePath("instance.ini"));
		settings->registerSetting("ConsoleMaxLines", 500);
		settings->registerSetting("ConsoleOverflowStop", false);

		settings->set("ConsoleMaxLines", 2500);
		QCOMPARE(getConsoleMaxLines(settings), 2500);

		settings->set("ConsoleMaxLines", "nope");
		QCOMPARE(getConsoleMaxLines(settings), 500);

		QVERIFY(!shouldStopOnConsoleOverflow(settings));
		settings->set("ConsoleOverflowStop", true);
		QVERIFY(shouldStopOnConsoleOverflow(settings));
	}
};

QTEST_GUILESS_MAIN(BaseInstanceSettingsTest)

#include "BaseInstanceSettings_test.moc"
