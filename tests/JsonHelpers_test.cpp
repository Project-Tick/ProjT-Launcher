// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 Project Tick
// SPDX-FileContributor: Project Tick Team

#include <QTest>
#include <QVariantMap>

#include <Json.h>

class JsonHelpersTest : public QObject
{
	Q_OBJECT

  private slots:
	void test_stringListRoundTrip_data()
	{
		QTest::addColumn<QStringList>("input");

		QTest::newRow("empty") << QStringList{};
		QTest::newRow("single") << QStringList{ "a" };
		QTest::newRow("multi") << QStringList{ "one", "two", "three" };
		QTest::newRow("spaces") << QStringList{ "a b", " c ", "x=y" };
	}

	void test_stringListRoundTrip()
	{
		QFETCH(QStringList, input);
		QCOMPARE(Json::toStringList(Json::fromStringList(input)), input);
	}

	void test_stringListInvalid()
	{
		QCOMPARE(Json::toStringList("not-json"), QStringList{});
		QCOMPARE(Json::toStringList("{}"), QStringList{});
	}

	void test_mapRoundTrip()
	{
		QVariantMap map;
		map.insert("int", 5);
		map.insert("bool", true);
		map.insert("str", "abc");

		const QVariantMap out = Json::toMap(Json::fromMap(map));
		QCOMPARE(out.value("int").toInt(), 5);
		QCOMPARE(out.value("bool").toBool(), true);
		QCOMPARE(out.value("str").toString(), QString("abc"));
	}

	void test_mapInvalid()
	{
		QCOMPARE(Json::toMap("not-json"), QVariantMap{});
		QCOMPARE(Json::toMap("[]"), QVariantMap{});
	}
};

QTEST_GUILESS_MAIN(JsonHelpersTest)

#include "JsonHelpers_test.moc"
