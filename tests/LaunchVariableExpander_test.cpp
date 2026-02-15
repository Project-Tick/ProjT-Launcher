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

#include <QProcessEnvironment>
#include <QTest>

#include <launch/LaunchVariableExpander.hpp>

class LaunchVariableExpanderTest : public QObject
{
	Q_OBJECT

  private slots:
	void test_expandVariables()
	{
		projt::launch::LaunchVariableExpander expander;
		QProcessEnvironment env;
		env.insert("HOME", "/home/test");
		env.insert("USER", "bob");

		QCOMPARE(expander.expand("$HOME/bin", env), QString("/home/test/bin"));
		QCOMPARE(expander.expand("Hello $USER", env), QString("Hello bob"));
		QCOMPARE(expander.expand("${HOME}/.config", env), QString("/home/test/.config"));
		QCOMPARE(expander.expand("$USER:$HOME", env), QString("bob:/home/test"));
	}

	void test_missingVariablesRemain()
	{
		projt::launch::LaunchVariableExpander expander;
		QProcessEnvironment env;
		env.insert("HOME", "/home/test");

		QCOMPARE(expander.expand("$MISSING/path", env), QString("$MISSING/path"));
		QCOMPARE(expander.expand("${MISSING}/path", env), QString("${MISSING}/path"));
		QCOMPARE(expander.expand("$HOME/$MISSING", env), QString("/home/test/$MISSING"));
	}
};

QTEST_GUILESS_MAIN(LaunchVariableExpanderTest)

#include "LaunchVariableExpander_test.moc"
