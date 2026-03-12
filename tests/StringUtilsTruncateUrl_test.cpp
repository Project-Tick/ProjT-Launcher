// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 Project Tick
// SPDX-FileContributor: Project Tick Team

#include <QTest>
#include <QUrl>

#include <StringUtils.h>

class StringUtilsTruncateUrlTest : public QObject
{
	Q_OBJECT

  private slots:
	void test_noTruncationWhenLongEnough()
	{
		QUrl url("https://user:pass@example.com/one/two#frag");
		const QString out = StringUtils::truncateUrlHumanFriendly(url, 200);
		QCOMPARE(out, QString("https://example.com/one/two"));
	}

	void test_softTruncationKeepsTail()
	{
		QUrl url("https://example.com/alpha/bravo/charlie/delta");
		const QString out = StringUtils::truncateUrlHumanFriendly(url, 30, false);

		QVERIFY(out.contains("..."));
		QVERIFY(out.endsWith("/delta"));
		QVERIFY(out.startsWith("https://example.com"));
	}

	void test_hardLimitRespected()
	{
		QUrl url("https://example.com/very/long/path/for/testing/truncation");
		const QString out = StringUtils::truncateUrlHumanFriendly(url, 18, true);

		QVERIFY(out.size() <= 18);
		QVERIFY(out.endsWith("..."));
	}
};

QTEST_GUILESS_MAIN(StringUtilsTruncateUrlTest)

#include "StringUtilsTruncateUrl_test.moc"
