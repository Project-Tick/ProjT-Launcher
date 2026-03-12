// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 Project Tick
// SPDX-FileContributor: Project Tick Team

#include <QRegularExpression>
#include <QTest>

#include <StringUtils.h>

class StringUtilsSplitFirstTest : public QObject
{
	Q_OBJECT

  private slots:
	void test_splitStringSeparator()
	{
		auto p1 = StringUtils::splitFirst("name: value:rest", ": ");
		QCOMPARE(p1.first, QString("name"));
		QCOMPARE(p1.second, QString("value:rest"));

		auto p2 = StringUtils::splitFirst("a::b::c", "::");
		QCOMPARE(p2.first, QString("a"));
		QCOMPARE(p2.second, QString("b::c"));
	}

	void test_splitCharSeparator()
	{
		auto p = StringUtils::splitFirst("left/right/inner", '/');
		QCOMPARE(p.first, QString("left"));
		QCOMPARE(p.second, QString("right/inner"));
	}

	void test_splitRegexSeparator()
	{
		QRegularExpression re("\\d+");
		auto p1 = StringUtils::splitFirst("abc123def", re);
		QCOMPARE(p1.first, QString("abc"));
		QCOMPARE(p1.second, QString("def"));

		auto p2 = StringUtils::splitFirst("no_digits_here", re);
		QCOMPARE(p2.first, QString("no_digits_here"));
		QCOMPARE(p2.second, QString(""));
	}
};

QTEST_GUILESS_MAIN(StringUtilsSplitFirstTest)

#include "StringUtilsSplitFirst_test.moc"
