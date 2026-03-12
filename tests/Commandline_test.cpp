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

#include <QTest>

#include <Commandline.h>

class CommandlineTest : public QObject
{
	Q_OBJECT

  private slots:
	void test_splitArgs_data()
	{
		QTest::addColumn<QString>("input");
		QTest::addColumn<QStringList>("expected");

		QTest::newRow("empty") << "" << QStringList{};
		QTest::newRow("spaces-only") << "     " << QStringList{};
		QTest::newRow("single") << "java" << QStringList{ "java" };
		QTest::newRow("multiple-with-extra-spaces")
			<< "  java   -jar   app.jar "
			<< QStringList{ "java", "-jar", "app.jar" };
		QTest::newRow("double-quoted")
			<< "java -Dk=\"hello world\""
			<< QStringList{ "java", "-Dk=hello world" };
		QTest::newRow("single-quoted")
			<< "java '-Dk=hello world' tail"
			<< QStringList{ "java", "-Dk=hello world", "tail" };
		QTest::newRow("mixed-quotes")
			<< "cmd \"double quoted\" 'single quoted'"
			<< QStringList{ "cmd", "double quoted", "single quoted" };
		QTest::newRow("escaped-quote-in-double-quotes")
			<< "\"a\\\"b\""
			<< QStringList{ "a\"b" };
		QTest::newRow("escaped-backslash-in-double-quotes")
			<< "\"C:\\\\temp\\\\file.txt\""
			<< QStringList{ "C:\\temp\\file.txt" };
		QTest::newRow("unclosed-quote")
			<< "\"hello world"
			<< QStringList{ "hello world" };
		QTest::newRow("single-backslash-outside-quotes")
			<< "a\\ b"
			<< QStringList{ "a\\", "b" };
	}

	void test_splitArgs()
	{
		QFETCH(QString, input);
		QFETCH(QStringList, expected);

		QCOMPARE(Commandline::splitArgs(input), expected);
	}
};

QTEST_GUILESS_MAIN(CommandlineTest)

#include "Commandline_test.moc"
