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

#include <MMCTime.h>

class MMCTimeTest : public QObject
{
	Q_OBJECT

  private slots:
	void test_prettifyDuration_data()
	{
		QTest::addColumn<int64_t>("seconds");
		QTest::addColumn<bool>("noDays");
		QTest::addColumn<QString>("expected");

		QTest::newRow("seconds-only") << int64_t(59) << false << "0min 59s";
		QTest::newRow("minutes-and-seconds") << int64_t(125) << false << "2min 5s";
		QTest::newRow("hours") << int64_t(3661) << false << "1h 1min";
		QTest::newRow("days") << int64_t(90061) << false << "1d 1h 1min";
		QTest::newRow("noDays-folds-days-into-hours") << int64_t(90061) << true << "25h 1min";
	}

	void test_prettifyDuration()
	{
		QFETCH(int64_t, seconds);
		QFETCH(bool, noDays);
		QFETCH(QString, expected);

		QCOMPARE(Time::prettifyDuration(seconds, noDays), expected);
	}

	void test_humanReadableDuration_signAndUnits()
	{
		const auto zero = Time::humanReadableDuration(0.0);
		QCOMPARE(zero, QString("0ms"));

		const auto negative = Time::humanReadableDuration(-0.5, 1);
		QVERIFY(negative.startsWith('-'));
		QVERIFY(negative.contains("ms"));

		const auto withSeconds = Time::humanReadableDuration(65.0);
		QVERIFY(withSeconds.contains("m"));
		QVERIFY(withSeconds.contains("s"));

		const auto withDays = Time::humanReadableDuration(90061.0);
		QVERIFY(withDays.contains("days"));
		QVERIFY(withDays.contains("h"));
		QVERIFY(withDays.contains("m"));
		QVERIFY(withDays.contains("s"));
	}
};

QTEST_GUILESS_MAIN(MMCTimeTest)

#include "MMCTime_test.moc"
