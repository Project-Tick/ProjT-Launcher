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

#include <QSignalSpy>
#include <QTest>

#include <launch/LaunchPipeline.hpp>

class LaunchPipelineTest : public QObject
{
	Q_OBJECT

  private slots:
	void test_executeTaskWithoutInstance()
	{
		projt::launch::LaunchPipeline pipeline(nullptr);
		QSignalSpy failedSpy(&pipeline, &Task::failed);

		pipeline.start();
		QCOMPARE(failedSpy.count(), 1);
		QCOMPARE(failedSpy.takeFirst().at(0).toString(), QString("Missing instance for launch."));
	}

	void test_censorPrivateInfo()
	{
		projt::launch::LaunchPipeline pipeline(nullptr);
		pipeline.setCensorFilter({{"SECRET", "REDACTED"}});
		QCOMPARE(pipeline.censorPrivateInfo("token SECRET"), QString("token REDACTED"));
	}

	void test_canAbortIdle()
	{
		projt::launch::LaunchPipeline pipeline(nullptr);
		QVERIFY(pipeline.canAbort());
	}
};

QTEST_GUILESS_MAIN(LaunchPipelineTest)

#include "LaunchPipeline_test.moc"
