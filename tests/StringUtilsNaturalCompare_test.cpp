// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 Project Tick
// SPDX-FileContributor: Project Tick Team

#include <QTest>

#include <StringUtils.h>

class StringUtilsNaturalCompareTest : public QObject
{
	Q_OBJECT

  private slots:
	void test_compare_data()
	{
		QTest::addColumn<QString>("first");
		QTest::addColumn<QString>("second");
		QTest::addColumn<Qt::CaseSensitivity>("cs");
		QTest::addColumn<int>("expectedSign");

		QTest::newRow("number-order-1") << "file 2" << "file 10" << Qt::CaseSensitive << -1;
		QTest::newRow("number-order-2") << "mod9" << "mod10" << Qt::CaseSensitive << -1;
		QTest::newRow("reverse-number") << "mod10" << "mod9" << Qt::CaseSensitive << 1;
		QTest::newRow("leading-zero") << "file 02" << "file 2" << Qt::CaseSensitive << -1;
		QTest::newRow("spaces-ignored") << "a  1" << "a 2" << Qt::CaseSensitive << -1;
		QTest::newRow("case-insensitive-eq") << "Alpha42" << "aLpHa42" << Qt::CaseInsensitive << 0;
		QTest::newRow("suffix-alpha") << "a10a" << "a10b" << Qt::CaseSensitive << -1;
		QTest::newRow("different-prefix") << "b1" << "a9" << Qt::CaseSensitive << 1;
		QTest::newRow("equal") << "same" << "same" << Qt::CaseSensitive << 0;
	}

	void test_compare()
	{
		QFETCH(QString, first);
		QFETCH(QString, second);
		QFETCH(Qt::CaseSensitivity, cs);
		QFETCH(int, expectedSign);

		const int result = StringUtils::naturalCompare(first, second, cs);
		QVERIFY((expectedSign < 0 && result < 0) || (expectedSign == 0 && result == 0) ||
				(expectedSign > 0 && result > 0));
	}

	void test_caseSensitiveDifferentCaseIsNotEqual()
	{
		const int result = StringUtils::naturalCompare("Alpha", "alpha", Qt::CaseSensitive);
		QVERIFY(result != 0);
	}
};

QTEST_GUILESS_MAIN(StringUtilsNaturalCompareTest)

#include "StringUtilsNaturalCompare_test.moc"
