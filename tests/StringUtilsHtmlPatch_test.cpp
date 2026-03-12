// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 Project Tick
// SPDX-FileContributor: Project Tick Team

#include <QTest>

#include <StringUtils.h>

class StringUtilsHtmlPatchTest : public QObject
{
	Q_OBJECT

  private slots:
	void test_insertsBreakBeforeImage()
	{
		const QString html = "<ul><li>a</li></ul>   <img src=\"x\">";
		const QString patched = StringUtils::htmlListPatch(html);
		QVERIFY(patched.contains("</ul><br>"));
	}

	void test_noBreakWhenTextExists()
	{
		const QString html = "<ul></ul> text <img src=\"x\">";
		const QString patched = StringUtils::htmlListPatch(html);
		QVERIFY(!patched.contains("</ul><br>"));
	}

	void test_multipleLists()
	{
		const QString html = "<ul></ul> <img src=\"a\"><ul></ul> <img src=\"b\">";
		const QString patched = StringUtils::htmlListPatch(html);
		QCOMPARE(patched.count("</ul><br>"), 2);
	}
};

QTEST_GUILESS_MAIN(StringUtilsHtmlPatchTest)

#include "StringUtilsHtmlPatch_test.moc"
