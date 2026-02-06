// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 Project Tick
// SPDX-FileContributor: Project Tick Team

#include <QTest>

#include <java/core/RuntimeVersion.hpp>

using projt::java::RuntimeVersion;

class RuntimeVersionTest : public QObject
{
	Q_OBJECT

  private slots:
	void parseLegacyJava8()
	{
		RuntimeVersion v("1.8.0_312");
		QCOMPARE(v.major(), 8);
		QCOMPARE(v.minor(), 0);
		QCOMPARE(v.security(), 312);
		QCOMPARE(v.prerelease(), QString());
		QVERIFY(!v.needsPermGen());
		QVERIFY(!v.supportsModules());
	}

	void parseModernJava()
	{
		RuntimeVersion v("17.0.2");
		QCOMPARE(v.major(), 17);
		QCOMPARE(v.minor(), 0);
		QCOMPARE(v.security(), 2);
		QVERIFY(!v.needsPermGen());
		QVERIFY(v.supportsModules());
		QVERIFY(!v.defaultsToUtf8());
	}

	void parsePrerelease()
	{
		RuntimeVersion v("21.0.1-rc1");
		QCOMPARE(v.major(), 21);
		QCOMPARE(v.minor(), 0);
		QCOMPARE(v.security(), 1);
		QCOMPARE(v.prerelease(), QStringLiteral("rc1"));
		QVERIFY(v.defaultsToUtf8());
	}

	void parseInvalid()
	{
		RuntimeVersion v("not-a-version");
		QCOMPARE(v.major(), 0);
		QCOMPARE(v.security(), 0);
		QVERIFY(v.needsPermGen());
		QVERIFY(!v.supportsModules());
		QVERIFY(!v.defaultsToUtf8());
	}

	void compareVersions()
	{
		RuntimeVersion base("17.0.2");
		RuntimeVersion higher("17.0.3");
		RuntimeVersion prerelease("17.0.3-ea");

		QVERIFY(base < higher);
		QVERIFY(higher > base);
		QVERIFY(prerelease < higher);
		QVERIFY(!(prerelease == higher));  // Use negated == instead of !=
	}
};

QTEST_GUILESS_MAIN(RuntimeVersionTest)

#include "RuntimeVersion_test.moc"
