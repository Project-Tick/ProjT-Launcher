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

#include <MessageLevel.h>

class MessageLevelTest : public QObject
{
	Q_OBJECT

  private slots:
	void test_getLevelString()
	{
		QCOMPARE(MessageLevel::getLevel("info"), MessageLevel::Info);
		QCOMPARE(MessageLevel::getLevel("WARN"), MessageLevel::Warning);
		QCOMPARE(MessageLevel::getLevel("critical"), MessageLevel::Error);
		QCOMPARE(MessageLevel::getLevel("nope"), MessageLevel::Unknown);
	}

	void test_getLevelQtMsgType()
	{
		QCOMPARE(MessageLevel::getLevel(QtDebugMsg), MessageLevel::Debug);
		QCOMPARE(MessageLevel::getLevel(QtInfoMsg), MessageLevel::Info);
		QCOMPARE(MessageLevel::getLevel(QtWarningMsg), MessageLevel::Warning);
		QCOMPARE(MessageLevel::getLevel(QtCriticalMsg), MessageLevel::Error);
		QCOMPARE(MessageLevel::getLevel(QtFatalMsg), MessageLevel::Fatal);
	}

	void test_fromLine()
	{
		QString line = "!![INFO]! hello";
		QCOMPARE(MessageLevel::fromLine(line), MessageLevel::Info);
		QCOMPARE(line, QString(" hello"));

		QString noPrefix = "plain";
		QCOMPARE(MessageLevel::fromLine(noPrefix), MessageLevel::Unknown);
		QCOMPARE(noPrefix, QString("plain"));
	}

	void test_fromLauncherLine()
	{
		QString line = "12 INFO: message";
		QCOMPARE(MessageLevel::fromLauncherLine(line), MessageLevel::Info);
		QCOMPARE(line, QString("message"));

		QString bad = "nope";
		QCOMPARE(MessageLevel::fromLauncherLine(bad), MessageLevel::Unknown);
		QCOMPARE(bad, QString("nope"));
	}
};

QTEST_GUILESS_MAIN(MessageLevelTest)

#include "MessageLevel_test.moc"
