// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 Project Tick
// SPDX-FileContributor: Project Tick Team

#include <QSettings>
#include <QTemporaryDir>
#include <QTest>

#include <updater/ProjTExternalUpdater.h>

class ProjTExternalUpdaterTest : public QObject
{
	Q_OBJECT

  private slots:
	void defaultsFromEmptySettings()
	{
		QTemporaryDir tempDir;
		QVERIFY(tempDir.isValid());

		ProjTExternalUpdater updater(nullptr, tempDir.path(), tempDir.path());
		QCOMPARE(updater.getAutomaticallyChecksForUpdates(), false);
		QCOMPARE(updater.getBetaAllowed(), false);
		QCOMPARE(updater.getUpdateCheckInterval(), 86400.0);
	}

	void invalidIntervalFallsBackToDefault()
	{
		QTemporaryDir tempDir;
		QVERIFY(tempDir.isValid());

		const auto settingsPath = QDir(tempDir.path()).absoluteFilePath("projtlauncher_update.cfg");
		{
			QSettings settings(settingsPath, QSettings::IniFormat);
			settings.setValue("update_interval", "not-a-number");
			settings.sync();
		}

		ProjTExternalUpdater updater(nullptr, tempDir.path(), tempDir.path());
		QCOMPARE(updater.getUpdateCheckInterval(), 86400.0);
	}

	void settersUpdateState()
	{
		QTemporaryDir tempDir;
		QVERIFY(tempDir.isValid());

		ProjTExternalUpdater updater(nullptr, tempDir.path(), tempDir.path());
		updater.setUpdateCheckInterval(3600);
		updater.setAutomaticallyChecksForUpdates(true);
		updater.setBetaAllowed(true);

		QCOMPARE(updater.getUpdateCheckInterval(), 3600.0);
		QCOMPARE(updater.getAutomaticallyChecksForUpdates(), true);
		QCOMPARE(updater.getBetaAllowed(), true);
	}
};

QTEST_GUILESS_MAIN(ProjTExternalUpdaterTest)

#include "ProjTExternalUpdater_test.moc"
