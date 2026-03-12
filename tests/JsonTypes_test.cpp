// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 Project Tick
// SPDX-FileContributor: Project Tick Team

#include <QDir>
#include <QTest>

#include <Json.h>

class JsonTypesTest : public QObject
{
	Q_OBJECT

  private slots:
	void test_requireInteger_data()
	{
		QTest::addColumn<QJsonValue>("input");
		QTest::addColumn<bool>("throws");
		QTest::addColumn<int>("expected");

		QTest::newRow("int") << QJsonValue(42) << false << 42;
		QTest::newRow("negative-int") << QJsonValue(-11) << false << -11;
		QTest::newRow("float") << QJsonValue(1.2) << true << 0;
		QTest::newRow("string") << QJsonValue("42") << true << 0;
	}

	void test_requireInteger()
	{
		QFETCH(QJsonValue, input);
		QFETCH(bool, throws);
		QFETCH(int, expected);

		bool threw = false;
		int value = 0;
		try
		{
			value = Json::requireInteger(input, "int");
		}
		catch (const Json::JsonException&)
		{
			threw = true;
		}

		QCOMPARE(threw, throws);
		if (!throws)
		{
			QCOMPARE(value, expected);
		}
	}

	void test_requireUrl()
	{
		QCOMPARE(Json::requireUrl(QJsonValue(QString("")), "url"), QUrl());
		QVERIFY(Json::requireUrl(QJsonValue(QString("https://example.com/path")), "url").isValid());

		bool threw = false;
		try
		{
			Json::requireUrl(QJsonValue(QString("http://exa mple.com")), "url");
		}
		catch (const Json::JsonException&)
		{
			threw = true;
		}
		QVERIFY(threw);
	}

	void test_requireDir()
	{
		QDir dir = Json::requireDir(QJsonValue(QString("a/../b/test")), "dir");
		QVERIFY(dir.isAbsolute());
		QVERIFY(!dir.absolutePath().contains(".."));

		bool threw = false;
		try
		{
			Json::requireDir(QJsonValue(QString("/etc")), "dir");
		}
		catch (const Json::JsonException&)
		{
			threw = true;
		}
		QVERIFY(threw);
	}
};

QTEST_GUILESS_MAIN(JsonTypesTest)

#include "JsonTypes_test.moc"
