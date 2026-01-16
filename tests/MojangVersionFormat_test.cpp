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
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <QDebug>
#include <QTest>

#include <minecraft/MojangVersionFormat.h>

class MojangVersionFormatTest : public QObject
{
	Q_OBJECT

	static QJsonDocument readJson(const QString path)
	{
		QFile jsonFile(path);
		if (!jsonFile.open(QIODevice::ReadOnly))
		{
			qWarning() << "Failed to open file '" << jsonFile.fileName() << "' for reading!";
			return QJsonDocument();
		}
		auto data = jsonFile.readAll();
		jsonFile.close();
		return QJsonDocument::fromJson(data);
	}
	static void writeJson(const char* file, QJsonDocument doc)
	{
		QFile jsonFile(file);
		if (!jsonFile.open(QIODevice::WriteOnly | QIODevice::Text))
		{
			qCritical() << "Failed to open file '" << jsonFile.fileName() << "' for writing!";
			return;
		}
		auto data = doc.toJson(QJsonDocument::Indented);
		qDebug() << QString::fromUtf8(data);
		jsonFile.write(data);
		jsonFile.close();
	}

  private slots:
	void test_Through_Simple()
	{
		QJsonDocument doc = readJson(QFINDTESTDATA("testdata/MojangVersionFormat/1.9-simple.json"));
		auto vfile		  = MojangVersionFormat::versionFileFromJson(doc, "1.9-simple.json");
		auto doc2		  = MojangVersionFormat::versionFileToJson(vfile);
		writeJson("1.9-simple-passthorugh.json", doc2);

		QCOMPARE(doc.toJson(), doc2.toJson());
	}

	void test_Through()
	{
		QJsonDocument doc = readJson(QFINDTESTDATA("testdata/MojangVersionFormat/1.9.json"));
		auto vfile		  = MojangVersionFormat::versionFileFromJson(doc, "1.9.json");
		auto doc2		  = MojangVersionFormat::versionFileToJson(vfile);
		writeJson("1.9-passthorugh.json", doc2);
		QCOMPARE(doc.toJson(), doc2.toJson());
	}
};

QTEST_GUILESS_MAIN(MojangVersionFormatTest)

#include "MojangVersionFormat_test.moc"
