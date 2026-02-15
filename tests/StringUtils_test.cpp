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

#include <QRegularExpression>
#include <QTest>
#include <QUrl>

#include <StringUtils.h>

class StringUtilsTest : public QObject
{
	Q_OBJECT

  private slots:
	void test_naturalCompare_data()
	{
		QTest::addColumn<QString>("first");
		QTest::addColumn<QString>("second");
		QTest::addColumn<int>("expected");
		QTest::addColumn<Qt::CaseSensitivity>("caseSensitivity");

		QTest::newRow("numeric-order") << "file 2" << "file 10" << -1 << Qt::CaseSensitive;
		QTest::newRow("numeric-order-2") << "a9" << "a10" << -1 << Qt::CaseSensitive;
		QTest::newRow("leading-zeros") << "file 02" << "file 2" << -1 << Qt::CaseSensitive;
		QTest::newRow("case-insensitive") << "Alpha2" << "alpha10" << -1 << Qt::CaseInsensitive;
	}

	void test_naturalCompare()
	{
		QFETCH(QString, first);
		QFETCH(QString, second);
		QFETCH(int, expected);
		QFETCH(Qt::CaseSensitivity, caseSensitivity);

		const int result = StringUtils::naturalCompare(first, second, caseSensitivity);
		QVERIFY((expected < 0 && result < 0) || (expected == 0 && result == 0) || (expected > 0 && result > 0));
	}

	void test_truncateUrlHumanFriendly()
	{
		QUrl url_no_truncation("https://user:pass@example.com/one/two#frag");
		const QString display = StringUtils::truncateUrlHumanFriendly(url_no_truncation, 200);
		QCOMPARE(display, QString("https://example.com/one/two"));

		QUrl url_truncate("https://example.com/alpha/bravo/charlie/delta");
		const QString truncated = StringUtils::truncateUrlHumanFriendly(url_truncate, 30);
		QVERIFY(truncated.contains("..."));
		QVERIFY(truncated.endsWith("/delta"));
		QVERIFY(truncated.size() <= 30);

		QUrl url_hard("https://example.com/alpha/bravo/charlie/delta");
		const QString hard = StringUtils::truncateUrlHumanFriendly(url_hard, 15, true);
		QVERIFY(hard.size() <= 15);
		QVERIFY(hard.endsWith("..."));
	}

	void test_humanReadableFileSize()
	{
		QCOMPARE(StringUtils::humanReadableFileSize(1024.0, false), QString("1.00 KiB"));
		QCOMPARE(StringUtils::humanReadableFileSize(1000.0, true), QString("1.00 KB"));
	}

	void test_getRandomAlphaNumeric()
	{
		const QString first = StringUtils::getRandomAlphaNumeric();
		const QString second = StringUtils::getRandomAlphaNumeric();

		QVERIFY(first.size() == 32);
		QVERIFY(second.size() == 32);
		QVERIFY(first != second);

		QRegularExpression re("^[0-9a-fA-F]{32}$");
		QVERIFY(re.match(first).hasMatch());
		QVERIFY(re.match(second).hasMatch());
	}

	void test_splitFirst()
	{
		{
			auto result = StringUtils::splitFirst("version: 1.2.3", ": ");
			QCOMPARE(result.first, QString("version"));
			QCOMPARE(result.second, QString("1.2.3"));
		}
		{
			auto result = StringUtils::splitFirst("a:b:c", ':');
			QCOMPARE(result.first, QString("a"));
			QCOMPARE(result.second, QString("b:c"));
		}
		{
			QRegularExpression re("\\d+");
			auto result = StringUtils::splitFirst("ab12cd", re);
			QCOMPARE(result.first, QString("ab"));
			QCOMPARE(result.second, QString("cd"));
		}
		{
			QRegularExpression re("\\d+");
			auto result = StringUtils::splitFirst("abc", re);
			QCOMPARE(result.first, QString("abc"));
			QCOMPARE(result.second, QString(""));
		}
	}

	void test_htmlListPatch()
	{
		const QString html = "<ul><li>a</li></ul>   <img src=\"x\">";
		const QString patched = StringUtils::htmlListPatch(html);
		QVERIFY(patched.contains("</ul><br>"));
		QVERIFY(patched.contains("<img"));

		const QString html_with_text = "<ul></ul> text <img src=\"x\">";
		const QString patched_with_text = StringUtils::htmlListPatch(html_with_text);
		QVERIFY(!patched_with_text.contains("</ul><br>"));
	}
};

QTEST_GUILESS_MAIN(StringUtilsTest)

#include "StringUtils_test.moc"
