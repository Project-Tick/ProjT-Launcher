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

#include <launch/LaunchLogModel.hpp>

class LaunchLogModelTest : public QObject
{
	Q_OBJECT

  private slots:
	void test_appendAndToPlainText()
	{
		projt::launch::LaunchLogModel model;
		QCOMPARE(model.rowCount(), 0);

		model.append(MessageLevel::Info, "line1");
		model.append(MessageLevel::Warning, "line2");

		QCOMPARE(model.rowCount(), 2);
		const QString text = model.toPlainText();
		QVERIFY(text.contains("line1"));
		QVERIFY(text.contains("line2"));
	}

	void test_setMaxLinesKeepsTail()
	{
		projt::launch::LaunchLogModel model;
		model.setMaxLines(3);
		model.append(MessageLevel::Info, "a");
		model.append(MessageLevel::Info, "b");
		model.append(MessageLevel::Info, "c");
		model.append(MessageLevel::Info, "d");

		QCOMPARE(model.rowCount(), 3);
		const QString text = model.toPlainText();
		QVERIFY(!text.contains("a\n"));
		QVERIFY(text.contains("b\n"));
		QVERIFY(text.contains("c\n"));
		QVERIFY(text.contains("d\n"));
	}

	void test_overflowStops()
	{
		projt::launch::LaunchLogModel model;
		model.setMaxLines(2);
		model.setStopOnOverflow(true);
		model.setOverflowMessage("OVERFLOW");

		model.append(MessageLevel::Info, "first");
		model.append(MessageLevel::Info, "second");

		QCOMPARE(model.rowCount(), 2);
		QVERIFY(model.isOverflow());

		auto overflowIndex = model.index(1);
		QCOMPARE(model.data(overflowIndex, Qt::DisplayRole).toString(), QString("OVERFLOW"));
		QCOMPARE(model.data(overflowIndex, projt::launch::LaunchLogModel::LevelRole).toInt(),
				 MessageLevel::Fatal);

		model.append(MessageLevel::Info, "third");
		QCOMPARE(model.rowCount(), 2);
	}

	void test_suspendAndOptions()
	{
		projt::launch::LaunchLogModel model;
		model.suspend(true);
		model.append(MessageLevel::Info, "line");
		QCOMPARE(model.rowCount(), 0);
		model.suspend(false);
		model.append(MessageLevel::Info, "line");
		QCOMPARE(model.rowCount(), 1);

		model.setLineWrap(false);
		QVERIFY(!model.wrapLines());
		model.setColorLines(false);
		QVERIFY(!model.colorLines());
	}
};

QTEST_GUILESS_MAIN(LaunchLogModelTest)

#include "LaunchLogModel_test.moc"
