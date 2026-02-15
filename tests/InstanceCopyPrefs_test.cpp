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

#include <InstanceCopyPrefs.h>

class InstanceCopyPrefsTest : public QObject
{
	Q_OBJECT

  private slots:
	void test_defaults()
	{
		InstanceCopyPrefs prefs;
		QVERIFY(prefs.allTrue());
		QVERIFY(prefs.isCopySavesEnabled());
		QVERIFY(prefs.isKeepPlaytimeEnabled());
		QVERIFY(prefs.isCopyGameOptionsEnabled());
		QVERIFY(prefs.isCopyResourcePacksEnabled());
		QVERIFY(prefs.isCopyShaderPacksEnabled());
		QVERIFY(prefs.isCopyServersEnabled());
		QVERIFY(prefs.isCopyModsEnabled());
		QVERIFY(prefs.isCopyScreenshotsEnabled());
		QVERIFY(!prefs.isUseSymLinksEnabled());
		QVERIFY(!prefs.isUseHardLinksEnabled());
		QVERIFY(!prefs.isLinkRecursivelyEnabled());
		QVERIFY(!prefs.isDontLinkSavesEnabled());
		QVERIFY(!prefs.isUseCloneEnabled());
	}

	void test_filtersAndFlags()
	{
		InstanceCopyPrefs prefs;
		prefs.enableCopySaves(false);
		prefs.enableCopyMods(false);
		prefs.enableCopyServers(false);

		const QString regex = prefs.getSelectedFiltersAsRegex({"extra"});
		QVERIFY(regex.contains("[.]?minecraft/saves"));
		QVERIFY(regex.contains("[.]?minecraft/mods"));
		QVERIFY(regex.contains("[.]?minecraft/servers.dat"));
		QVERIFY(regex.contains("[.]?minecraft/extra"));
		QVERIFY(!prefs.allTrue());
	}

	void test_emptyFiltersWhenAllEnabled()
	{
		InstanceCopyPrefs prefs;
		QCOMPARE(prefs.getSelectedFiltersAsRegex(), QString(""));
	}
};

QTEST_GUILESS_MAIN(InstanceCopyPrefsTest)

#include "InstanceCopyPrefs_test.moc"
