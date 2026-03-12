// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 Project Tick
// SPDX-FileContributor: Project Tick Team

#include <QTest>

#include <Filter.h>

class FilterTest : public QObject
{
	Q_OBJECT

  private slots:
	void test_equalsAndContains()
	{
		const auto equals = Filters::equals("alpha");
		const auto contains = Filters::contains("bet");

		QVERIFY(equals("alpha"));
		QVERIFY(!equals("alphA"));
		QVERIFY(contains("alphabet"));
		QVERIFY(!contains("gamma"));
	}

	void test_anyAndInverse()
	{
		const auto filter = Filters::any({ Filters::equals("one"), Filters::startsWith("two") });
		const auto inverse = Filters::inverse(filter);

		QVERIFY(filter("one"));
		QVERIFY(filter("twofold"));
		QVERIFY(!filter("three"));

		QVERIFY(!inverse("one"));
		QVERIFY(inverse("three"));
	}

	void test_equalsAnyAndEqualsOrEmpty()
	{
		const auto any = Filters::equalsAny({ "java", "system" });
		const auto any_empty = Filters::equalsAny();
		const auto empty_or_exact = Filters::equalsOrEmpty("release");

		QVERIFY(any("java"));
		QVERIFY(!any("other"));
		QVERIFY(any_empty("anything"));

		QVERIFY(empty_or_exact(""));
		QVERIFY(empty_or_exact("release"));
		QVERIFY(!empty_or_exact("debug"));
	}

	void test_regexp()
	{
		const auto filter = Filters::regexp(QRegularExpression("^mod-[0-9]+$"));

		QVERIFY(filter("mod-123"));
		QVERIFY(!filter("mod-abc"));
	}
};

QTEST_GUILESS_MAIN(FilterTest)

#include "Filter_test.moc"
