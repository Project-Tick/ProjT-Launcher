// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 Project Tick
// SPDX-FileContributor: Project Tick Team

#include <QJsonObject>
#include <QTest>

#include <ApplicationMessage.h>
#include <Json.h>

class ApplicationMessageTest : public QObject
{
	Q_OBJECT

  private slots:
	void test_roundTrip_data()
	{
		QTest::addColumn<QString>("command");
		QTest::addColumn<QStringList>("keys");
		QTest::addColumn<QStringList>("values");

		QTest::newRow("empty-args") << "open" << QStringList{} << QStringList{};
		QTest::newRow("single-arg") << "launch" << QStringList{ "instance" } << QStringList{ "main" };
		QTest::newRow("multi-args") << "import"
								  << QStringList{ "path", "mode", "force" }
								  << QStringList{ "/tmp/test", "zip", "true" };
		QTest::newRow("symbols") << "cmd"
							<< QStringList{ "k-1", "k_2" }
							<< QStringList{ "value with spaces", "x=y" };
	}

	void test_roundTrip()
	{
		QFETCH(QString, command);
		QFETCH(QStringList, keys);
		QFETCH(QStringList, values);

		QCOMPARE(keys.size(), values.size());

		ApplicationMessage in;
		in.command = command;
		for (int i = 0; i < keys.size(); ++i)
		{
			in.args.insert(keys.at(i), values.at(i));
		}

		ApplicationMessage out;
		out.parse(in.serialize());

		QCOMPARE(out.command, command);
		QCOMPARE(out.args.size(), keys.size());
		for (int i = 0; i < keys.size(); ++i)
		{
			QCOMPARE(out.args.value(keys.at(i)), values.at(i));
		}
	}

	void test_parseMissingArgs()
	{
		QJsonObject root;
		root.insert("command", "only-command");

		ApplicationMessage msg;
		msg.parse(Json::toText(root));

		QCOMPARE(msg.command, QString("only-command"));
		QVERIFY(msg.args.isEmpty());
	}

	void test_parseInvalidDocumentThrows()
	{
		ApplicationMessage msg;
		bool threw = false;
		try
		{
			msg.parse("{");
		}
		catch (const Json::JsonException&)
		{
			threw = true;
		}
		QVERIFY(threw);
	}

	void test_parseArrayThrows()
	{
		ApplicationMessage msg;
		bool threw = false;
		try
		{
			msg.parse("[1,2,3]");
		}
		catch (const Json::JsonException&)
		{
			threw = true;
		}
		QVERIFY(threw);
	}
};

QTEST_GUILESS_MAIN(ApplicationMessageTest)

#include "ApplicationMessage_test.moc"
