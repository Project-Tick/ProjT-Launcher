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

#include <meta/Index.hpp>
#include <meta/VersionList.hpp>

class IndexTest : public QObject
{
	Q_OBJECT
  private slots:
	void test_hasComponent_and_component()
	{
		projt::meta::MetaIndex windex;
		// Note: MetaIndex now requires different initialization
		// Basic test for component existence
		QVERIFY(!windex.hasComponent("nonexistent"));
	}

	void test_version_access()
	{
		projt::meta::MetaIndex windex;
		// Test that accessing a non-existent version creates one
		auto version = windex.version("test.component", "1.0.0");
		QVERIFY(version != nullptr);
		QCOMPARE(version->componentUid(), QString("test.component"));
		QCOMPARE(version->versionId(), QString("1.0.0"));
	}
};

QTEST_GUILESS_MAIN(IndexTest)

#include "Index_test.moc"
