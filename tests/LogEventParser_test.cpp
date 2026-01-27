// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 Project Tick
// SPDX-FileContributor: Project Tick Team

#include <QTest>

#include <logs/LogEventParser.hpp>

using projt::logs::LogEventParser;

class LogEventParserTest : public QObject
{
	Q_OBJECT

  private slots:
	void parseLog4jComplete()
	{
		LogEventParser parser;
		const QString xml =
			"<log4j:Event logger=\"Test.Logger\" timestamp=\"1700000000\" level=\"WARN\" thread=\"Main\">"
			"<log4j:Message>Hello World</log4j:Message>"
			"</log4j:Event>";

		parser.pushLine(xml);
		auto item = parser.popNext();
		QVERIFY(item.has_value());
		QVERIFY(std::holds_alternative<LogEventParser::LogRecord>(item.value()));

		auto entry = std::get<LogEventParser::LogRecord>(item.value());
		QCOMPARE(entry.logger, QStringLiteral("Test.Logger"));
		QCOMPARE(entry.levelText, QStringLiteral("WARN"));
		QCOMPARE(entry.level, MessageLevel::Warning);
		QCOMPARE(entry.thread, QStringLiteral("Main"));
		QCOMPARE(entry.message, QStringLiteral("Hello World"));
		QCOMPARE(entry.timestamp.toSecsSinceEpoch(), 1700000000LL);
	}

	void parseLog4jMissingLogger()
	{
		LogEventParser parser;
		const QString xml = "<log4j:Event timestamp=\"1700000000\" level=\"INFO\" thread=\"Main\">"
							"<log4j:Message>Missing logger</log4j:Message>"
							"</log4j:Event>";

		parser.pushLine(xml);
		auto item = parser.popNext();
		QVERIFY(!item.has_value());
		auto err = parser.lastError();
		QVERIFY(err.has_value());
		QVERIFY(err->message.contains(QStringLiteral("missing logger"), Qt::CaseInsensitive));
	}

	void parseLog4jPendingChunk()
	{
		LogEventParser parser;
		const QString partial =
			"<log4j:Event logger=\"Test.Logger\" timestamp=\"1700000000\" level=\"INFO\" thread=\"Main\">"
			"<log4j:Message>Partial";
		const QString remainder = "</log4j:Message></log4j:Event>";

		parser.pushLine(partial);
		auto first = parser.popNext();
		QVERIFY(first.has_value());
		QVERIFY(std::holds_alternative<LogEventParser::PendingChunk>(first.value()));

		parser.pushLine(remainder);
		auto second = parser.popNext();
		QVERIFY(second.has_value());
		QVERIFY(std::holds_alternative<LogEventParser::LogRecord>(second.value()));
		auto entry = std::get<LogEventParser::LogRecord>(second.value());
		QCOMPARE(entry.message, QStringLiteral("Partial"));
	}

	void parseLog4jLeadingWhitespace()
	{
		LogEventParser parser;
		const QString xml =
			"   <log4j:Event logger=\"Test.Logger\" timestamp=\"1700000000\" level=\"INFO\" thread=\"Main\">"
			"<log4j:Message>Whitespace</log4j:Message>"
			"</log4j:Event>";

		parser.pushLine(xml);
		auto item = parser.popNext();
		QVERIFY(item.has_value());
		QVERIFY(std::holds_alternative<LogEventParser::LogRecord>(item.value()));
	}

	void guessLevelFromLine_data()
	{
		QTest::addColumn<QString>("line");
		QTest::addColumn<int>("fallback");
		QTest::addColumn<int>("expected");

		QTest::newRow("header-info") << "[12:34:56] [Server thread/INFO] Hello"
									 << static_cast<int>(MessageLevel::Unknown) << static_cast<int>(MessageLevel::Info);

		QTest::newRow("bracket-severe") << "[SEVERE] Something bad" << static_cast<int>(MessageLevel::Unknown)
										<< static_cast<int>(MessageLevel::Error);

		QTest::newRow("warning-tag") << "[WARNING] Potential issue" << static_cast<int>(MessageLevel::Unknown)
									 << static_cast<int>(MessageLevel::Warning);

		QTest::newRow("fatal-overwrite") << "overwriting existing" << static_cast<int>(MessageLevel::Unknown)
										 << static_cast<int>(MessageLevel::Fatal);

		QTest::newRow("fallback-preserved")
			<< "overwriting existing" << static_cast<int>(MessageLevel::Info) << static_cast<int>(MessageLevel::Info);
	}

	void guessLevelFromLine()
	{
		QFETCH(QString, line);
		QFETCH(int, fallback);
		QFETCH(int, expected);

		auto result = LogEventParser::guessLevelFromLine(line, static_cast<MessageLevel::Enum>(fallback));
		QCOMPARE(static_cast<int>(result), expected);
	}
};

QTEST_GUILESS_MAIN(LogEventParserTest)

#include "LogEventParser_test.moc"
