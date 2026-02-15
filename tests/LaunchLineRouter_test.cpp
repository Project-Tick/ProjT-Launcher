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

#include <QMap>
#include <QTest>

#include <launch/LaunchLineRouter.hpp>
#include <launch/LaunchLogModel.hpp>

class LaunchLineRouterTest : public QObject
{
	Q_OBJECT

  private slots:
	void test_routeLineWithRedaction()
	{
		projt::launch::LaunchLineRouter router;
		projt::launch::LaunchLogModel model;

		QMap<QString, QString> redactions;
		redactions.insert("SECRET", "REDACTED");

		QString line = "!![INFO]! user SECRET";
		router.routeLine(line, MessageLevel::Unknown, model, redactions);

		QCOMPARE(model.rowCount(), 1);
		QCOMPARE(model.data(model.index(0), Qt::DisplayRole).toString(), QString(" user REDACTED"));
		QCOMPARE(model.data(model.index(0), projt::launch::LaunchLogModel::LevelRole).toInt(),
				 MessageLevel::Info);
	}
};

QTEST_GUILESS_MAIN(LaunchLineRouterTest)

#include "LaunchLineRouter_test.moc"
