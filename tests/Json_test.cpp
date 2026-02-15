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

#include <QDateTime>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QTemporaryDir>
#include <QTest>
#include <QUuid>

#include <Json.h>

class JsonTest : public QObject
{
	Q_OBJECT

  private slots:
	void test_requireDocument_valid()
	{
		const QByteArray json = "{\"a\":1}";
		QJsonDocument doc = Json::requireDocument(json, "doc");
		QVERIFY(doc.isObject());
		QCOMPARE(doc.object().value("a").toInt(), 1);
	}

	void test_requireDocument_invalid()
	{
		bool threw = false;
		try {
			Json::requireDocument(QByteArray("{"), "doc");
		} catch (const Json::JsonException&) {
			threw = true;
		}
		QVERIFY(threw);
	}

	void test_requireDocument_binary()
	{
		QByteArray binary;
		auto tag = QJsonDocument::BinaryFormatTag;
		binary.append(reinterpret_cast<const char*>(&tag), sizeof(tag));
		binary.append("junk");

		bool threw = false;
		try {
			Json::requireDocument(binary, "doc");
		} catch (const Json::JsonException&) {
			threw = true;
		}
		QVERIFY(threw);
	}

	void test_requireObjectAndArray()
	{
		QJsonDocument objDoc(QJsonObject{{"a", 1}});
		QJsonObject obj = Json::requireObject(objDoc, "obj");
		QCOMPARE(obj.value("a").toInt(), 1);

		QJsonDocument arrayDoc(QJsonArray{1, 2});
		QJsonArray arr = Json::requireArray(arrayDoc, "arr");
		QCOMPARE(arr.size(), 2);
	}

	void test_writeStringAndList()
	{
		QJsonObject obj;
		Json::writeString(obj, "name", "value");
		Json::writeString(obj, "empty", "");
		QVERIFY(obj.contains("name"));
		QVERIFY(!obj.contains("empty"));

		Json::writeStringList(obj, "list", QStringList{"a", "b"});
		Json::writeStringList(obj, "list_empty", {});
		QVERIFY(obj.contains("list"));
		QVERIFY(!obj.contains("list_empty"));
	}

	void test_toTextRoundtrip()
	{
		QJsonObject obj{{"a", 1}, {"b", "x"}};
		QByteArray text = Json::toText(obj);
		QJsonDocument roundtrip = QJsonDocument::fromJson(text);
		QVERIFY(roundtrip.isObject());
		QCOMPARE(roundtrip.object().value("a").toInt(), 1);
		QCOMPARE(roundtrip.object().value("b").toString(), QString("x"));
	}

	void test_requireIsTypeBasics()
	{
		QJsonValue intValue(3);
		QCOMPARE(Json::requireInteger(intValue, "int"), 3);

		bool threw = false;
		try {
			Json::requireInteger(QJsonValue(1.5), "int");
		} catch (const Json::JsonException&) {
			threw = true;
		}
		QVERIFY(threw);

		QJsonValue boolValue(true);
		QCOMPARE(Json::requireBoolean(boolValue, "bool"), true);
	}

	void test_requireByteArray()
	{
		QJsonValue value(QString("ff"));
		QByteArray bytes = Json::requireByteArray(value, "bytes");
		QCOMPARE(bytes, QByteArray::fromHex("ff"));

		bool threw = false;
		try {
			Json::requireByteArray(QJsonValue(QString::fromUtf8("Ж")), "bytes");
		} catch (const Json::JsonException&) {
			threw = true;
		}
		QVERIFY(threw);
	}

	void test_requireUrlAndUuid()
	{
		QJsonValue emptyUrl(QString(""));
		QCOMPARE(Json::requireUrl(emptyUrl, "url"), QUrl());

		QJsonValue goodUrl(QString("https://example.com/test"));
		QCOMPARE(Json::requireUrl(goodUrl, "url").toString(), QString("https://example.com/test"));

		const QString uuidStr = QUuid::createUuid().toString();
		QCOMPARE(Json::requireUuid(QJsonValue(uuidStr), "uuid").toString(), uuidStr);

		bool threw = false;
		try {
			Json::requireUuid(QJsonValue(QString("not-a-uuid")), "uuid");
		} catch (const Json::JsonException&) {
			threw = true;
		}
		QVERIFY(threw);
	}

	void test_requireDir()
	{
		QJsonValue rel(QString("foo/../bar"));
		QDir dir = Json::requireDir(rel, "dir");
		QVERIFY(dir.isAbsolute());
		QVERIFY(!dir.absolutePath().contains(".."));

		bool threw = false;
		try {
			Json::requireDir(QJsonValue(QString("/etc")), "dir");
		} catch (const Json::JsonException&) {
			threw = true;
		}
		QVERIFY(threw);
	}

	void test_stringListAndMapHelpers()
	{
		const QStringList list{"a", "b"};
		const QString jsonList = Json::fromStringList(list);
		QCOMPARE(Json::toStringList(jsonList), list);

		QVariantMap map;
		map.insert("a", 1);
		map.insert("b", "x");
		const QString jsonMap = Json::fromMap(map);
		QVariantMap roundtrip = Json::toMap(jsonMap);
		QCOMPARE(roundtrip.value("a").toInt(), 1);
		QCOMPARE(roundtrip.value("b").toString(), QString("x"));

		QCOMPARE(Json::toStringList("not-json"), QStringList{});
		QCOMPARE(Json::toMap("not-json"), QVariantMap{});
	}

	void test_writeAndReadFile()
	{
		QTemporaryDir dir;
		QVERIFY(dir.isValid());
		const QString path = dir.filePath("test.json");

		QJsonObject obj{{"a", 5}};
		Json::write(obj, path);
		QJsonDocument doc = Json::requireDocument(path, "doc");
		QVERIFY(doc.isObject());
		QCOMPARE(doc.object().value("a").toInt(), 5);
	}
};

QTEST_GUILESS_MAIN(JsonTest)

#include "Json_test.moc"
