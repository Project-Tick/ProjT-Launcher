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

#include <minecraft/VersionFilterData.h>

class VersionFilterDataTest : public QObject
{
	Q_OBJECT

  private slots:
	void test_hasKnownVersionMappings()
	{
		QVERIFY(g_VersionFilterData.fmlLibsMapping.contains("1.3.2"));
		QVERIFY(g_VersionFilterData.fmlLibsMapping.contains("1.4.7"));
		QVERIFY(g_VersionFilterData.fmlLibsMapping.contains("1.5.2"));

		const auto libs15 = g_VersionFilterData.fmlLibsMapping.value("1.5.2");
		QVERIFY(!libs15.isEmpty());
		QCOMPARE(libs15.front().filename, QString("argo-small-3.2.jar"));
		QCOMPARE(libs15.back().filename, QString("scala-library.jar"));
	}

	void test_allMappedLibrariesHaveFilenameAndChecksum()
	{
		for (auto it = g_VersionFilterData.fmlLibsMapping.constBegin(); it != g_VersionFilterData.fmlLibsMapping.constEnd();
			 ++it)
		{
			QVERIFY2(!it.key().isEmpty(), "Minecraft version key must not be empty");
			for (const auto& lib : it.value())
			{
				QVERIFY2(!lib.filename.isEmpty(), "Library filename must not be empty");
				QVERIFY2(!lib.checksum.isEmpty(), "Library checksum must not be empty");
			}
		}
	}

	void test_installerBlacklist()
	{
		QVERIFY(g_VersionFilterData.forgeInstallerBlacklist.contains("1.5.2"));
		QVERIFY(!g_VersionFilterData.forgeInstallerBlacklist.contains("1.5.1"));
	}

	void test_lwjglWhitelistContainsExpectedEntries()
	{
		QVERIFY(g_VersionFilterData.lwjglWhitelist.contains("org.lwjgl.lwjgl:lwjgl"));
		QVERIFY(g_VersionFilterData.lwjglWhitelist.contains("org.lwjgl.lwjgl:lwjgl_util"));
		QVERIFY(g_VersionFilterData.lwjglWhitelist.contains("org.lwjgl.lwjgl:lwjgl-platform"));
	}

	void test_javaTransitionDatesAreOrdered()
	{
		QVERIFY(g_VersionFilterData.java8BeginsDate.isValid());
		QVERIFY(g_VersionFilterData.java16BeginsDate.isValid());
		QVERIFY(g_VersionFilterData.java17BeginsDate.isValid());

		QVERIFY(g_VersionFilterData.java8BeginsDate < g_VersionFilterData.java16BeginsDate);
		QVERIFY(g_VersionFilterData.java16BeginsDate < g_VersionFilterData.java17BeginsDate);
	}
};

QTEST_GUILESS_MAIN(VersionFilterDataTest)

#include "VersionFilterData_test.moc"
