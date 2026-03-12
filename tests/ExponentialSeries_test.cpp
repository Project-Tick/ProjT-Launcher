// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 Project Tick
// SPDX-FileContributor: Project Tick Team

#include <QTest>

#include <ExponentialSeries.h>

class ExponentialSeriesTest : public QObject
{
	Q_OBJECT

  private slots:
	void test_sequenceGrowsExponentially()
	{
		ExponentialSeries series(2, 32, 2);

		QCOMPARE(series(), 2u);
		QCOMPARE(series(), 4u);
		QCOMPARE(series(), 8u);
		QCOMPARE(series(), 16u);
	}

	void test_sequenceClampsAtMaximum()
	{
		ExponentialSeries series(3, 20, 3);

		QCOMPARE(series(), 3u);
		QCOMPARE(series(), 9u);
		QCOMPARE(series(), 20u);
		QCOMPARE(series(), 20u);
	}

	void test_resetRestoresMinimum()
	{
		ExponentialSeries series(5, 100, 4);

		QCOMPARE(series(), 5u);
		QCOMPARE(series(), 20u);

		series.reset();

		QCOMPARE(series(), 5u);
	}
};

QTEST_GUILESS_MAIN(ExponentialSeriesTest)

#include "ExponentialSeries_test.moc"
