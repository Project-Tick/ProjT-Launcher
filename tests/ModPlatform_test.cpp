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

#include <QSet>
#include <QTest>

#include <modplatform/ModIndex.h>
#include <modplatform/ResourceType.h>

class ModPlatformTest : public QObject
{
	Q_OBJECT

  private slots:
	void test_resourceTypeNames()
	{
		using ModPlatform::ResourceType;
		QCOMPARE(ModPlatform::ResourceTypeUtils::getName(ResourceType::Mod), QString("mod"));
		QCOMPARE(ModPlatform::ResourceTypeUtils::getName(ResourceType::ResourcePack), QString("resource pack"));
		QCOMPARE(ModPlatform::ResourceTypeUtils::getName(ResourceType::Unknown), QString("unknown"));
	}

	void test_versionTypeConversions()
	{
		using ModPlatform::IndexedVersionType;
		QCOMPARE(IndexedVersionType::enumFromString("release"), IndexedVersionType::VersionType::Release);
		QCOMPARE(IndexedVersionType::enumFromString("beta"), IndexedVersionType::VersionType::Beta);
		QCOMPARE(IndexedVersionType::enumFromString("alpha"), IndexedVersionType::VersionType::Alpha);
		QCOMPARE(IndexedVersionType::enumFromString("nope"), IndexedVersionType::VersionType::Unknown);
		QCOMPARE(IndexedVersionType::toString(IndexedVersionType::VersionType::Release), QString("release"));
		QCOMPARE(IndexedVersionType::toString(IndexedVersionType::VersionType::Unknown), QString("unknown"));
	}

	void test_providerCapabilities()
	{
		using ModPlatform::ProviderCapabilities::hashType;
		using ModPlatform::ProviderCapabilities::name;
		using ModPlatform::ProviderCapabilities::readableName;
		using ModPlatform::ResourceProvider;

		QCOMPARE(QString(name(ResourceProvider::MODRINTH)), QString("modrinth"));
		QCOMPARE(readableName(ResourceProvider::MODRINTH), QString("Modrinth"));
		QCOMPARE(hashType(ResourceProvider::MODRINTH), QStringList({"sha512", "sha1"}));

		QCOMPARE(QString(name(ResourceProvider::FLAME)), QString("curseforge"));
		QCOMPARE(readableName(ResourceProvider::FLAME), QString("CurseForge"));
		QCOMPARE(hashType(ResourceProvider::FLAME), QStringList({"sha1", "md5", "murmur2"}));
	}

	void test_metaUrl()
	{
		QCOMPARE(ModPlatform::getMetaURL(ModPlatform::ResourceProvider::FLAME, "123"),
				 QString("https://www.curseforge.com/projects/123"));
		QCOMPARE(ModPlatform::getMetaURL(ModPlatform::ResourceProvider::MODRINTH, "abc"),
				 QString("https://modrinth.com/mod/abc"));
	}

	void test_modLoaderStrings()
	{
		QCOMPARE(ModPlatform::getModLoaderAsString(ModPlatform::Forge), QString("forge"));
		QCOMPARE(ModPlatform::getModLoaderFromString("forge"), ModPlatform::Forge);
		QCOMPARE(ModPlatform::getModLoaderFromString("unknown"), static_cast<ModPlatform::ModLoaderType>(0));
	}

	void test_modLoaderFlagsToList()
	{
		ModPlatform::ModLoaderTypes flags;
		flags |= ModPlatform::Forge;
		flags |= ModPlatform::Fabric;
		flags |= ModPlatform::Optifine;
		QList<ModPlatform::ModLoaderType> list = ModPlatform::modLoaderTypesToList(flags);
		QSet<int> values;
		for (auto entry : list) {
			values.insert(static_cast<int>(entry));
		}
		QCOMPARE(values.size(), 3);
		QVERIFY(values.contains(static_cast<int>(ModPlatform::Forge)));
		QVERIFY(values.contains(static_cast<int>(ModPlatform::Fabric)));
		QVERIFY(values.contains(static_cast<int>(ModPlatform::Optifine)));
	}

	void test_sideConversions()
	{
		QCOMPARE(ModPlatform::SideUtils::fromString("client"), ModPlatform::Side::ClientSide);
		QCOMPARE(ModPlatform::SideUtils::toString(ModPlatform::Side::ClientSide), QString("client"));
		QCOMPARE(ModPlatform::SideUtils::fromString("server"), ModPlatform::Side::ServerSide);
		QCOMPARE(ModPlatform::SideUtils::toString(ModPlatform::Side::ServerSide), QString("server"));
		QCOMPARE(ModPlatform::SideUtils::fromString("both"), ModPlatform::Side::UniversalSide);
		QCOMPARE(ModPlatform::SideUtils::toString(ModPlatform::Side::UniversalSide), QString("both"));
		QCOMPARE(ModPlatform::SideUtils::fromString("unknown"), ModPlatform::Side::UniversalSide);
	}
};

QTEST_GUILESS_MAIN(ModPlatformTest)

#include "ModPlatform_test.moc"
