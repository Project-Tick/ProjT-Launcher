// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 Project Tick
// SPDX-FileContributor: Project Tick Team

#include <QTest>

#include <DefaultVariable.h>

class DefaultVariableTest : public QObject
{
	Q_OBJECT

  private slots:
	void test_initialState()
	{
		DefaultVariable<int> value(7);

		QVERIFY(value.isDefault());
		QVERIFY(!value.isExplicit());
		QCOMPARE(static_cast<int>(value), 7);
	}

	void test_assignmentMarksExplicit()
	{
		DefaultVariable<QString> value("alpha");
		value = "beta";

		QVERIFY(!value.isDefault());
		QVERIFY(value.isExplicit());
		QCOMPARE(static_cast<QString>(value), QString("beta"));
	}

	void test_assignmentToDefaultRestoresDefaultFlag()
	{
		DefaultVariable<QString> value("alpha");
		value = "beta";
		value = "alpha";

		QVERIFY(value.isDefault());
		QVERIFY(value.isExplicit());
		QCOMPARE(static_cast<QString>(value), QString("alpha"));
	}
};

QTEST_GUILESS_MAIN(DefaultVariableTest)

#include "DefaultVariable_test.moc"
