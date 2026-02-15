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

#include <QNetworkRequest>
#include <QTest>

#include <net/ByteArraySink.h>
#include <net/Validator.h>
#include <tasks/Task.h>

class DummyReply : public QNetworkReply
{
	Q_OBJECT

  public:
	explicit DummyReply(QObject* parent = nullptr) : QNetworkReply(parent)
	{
		setUrl(QUrl("https://example.com/"));
		open(QIODevice::ReadOnly);
		setFinished(true);
		setError(QNetworkReply::NoError, {});
	}

	void abort() override {}

  protected:
	qint64 readData(char*, qint64) override { return -1; }
};

class DummyValidator : public Net::Validator
{
  public:
	bool initResult = true;
	bool writeResult = true;
	bool abortResult = true;
	bool validateResult = true;

	bool init(QNetworkRequest&) override { return initResult; }
	bool write(QByteArray&) override { return writeResult; }
	bool abort() override { return abortResult; }
	bool validate(QNetworkReply&) override { return validateResult; }
};

class NetSinkTest : public QObject
{
	Q_OBJECT

  private slots:
	void test_byteArraySinkHappyPath()
	{
		auto buffer = std::make_shared<QByteArray>("preset");
		Net::ByteArraySink sink(buffer);
		sink.addValidator(new DummyValidator());

		QNetworkRequest request(QUrl("https://example.com"));
		QCOMPARE(sink.init(request), Task::State::Running);
		QCOMPARE(*buffer, QByteArray());

		QByteArray data("abc");
		QCOMPARE(sink.write(data), Task::State::Running);
		QCOMPARE(*buffer, QByteArray("abc"));

		DummyReply reply;
		QCOMPARE(sink.finalize(reply), Task::State::Succeeded);
		QVERIFY(!sink.hasLocalData());
	}

	void test_byteArraySinkInitFailure()
	{
		auto buffer = std::make_shared<QByteArray>();
		Net::ByteArraySink sink(buffer);
		auto validator = new DummyValidator();
		validator->initResult = false;
		sink.addValidator(validator);

		QNetworkRequest request(QUrl("https://example.com"));
		QCOMPARE(sink.init(request), Task::State::Failed);
		QCOMPARE(sink.failReason(), QString("Failed to initialize validators"));
	}

	void test_byteArraySinkWriteFailure()
	{
		auto buffer = std::make_shared<QByteArray>();
		Net::ByteArraySink sink(buffer);
		auto validator = new DummyValidator();
		validator->writeResult = false;
		sink.addValidator(validator);

		QNetworkRequest request(QUrl("https://example.com"));
		QCOMPARE(sink.init(request), Task::State::Running);

		QByteArray data("abc");
		QCOMPARE(sink.write(data), Task::State::Failed);
		QCOMPARE(sink.failReason(), QString("Failed to write validators"));
	}

	void test_byteArraySinkFinalizeFailure()
	{
		auto buffer = std::make_shared<QByteArray>();
		Net::ByteArraySink sink(buffer);
		auto validator = new DummyValidator();
		validator->validateResult = false;
		sink.addValidator(validator);

		QNetworkRequest request(QUrl("https://example.com"));
		QCOMPARE(sink.init(request), Task::State::Running);

		QByteArray data("abc");
		QCOMPARE(sink.write(data), Task::State::Running);

		DummyReply reply;
		QCOMPARE(sink.finalize(reply), Task::State::Failed);
		QCOMPARE(sink.failReason(), QString("Failed to finalize validators"));
	}

	void test_byteArraySinkAbort()
	{
		auto buffer = std::make_shared<QByteArray>();
		Net::ByteArraySink sink(buffer);
		sink.addValidator(new DummyValidator());

		QCOMPARE(sink.abort(), Task::State::Failed);
		QCOMPARE(sink.failReason(), QString("Aborted"));
	}
};

QTEST_GUILESS_MAIN(NetSinkTest)

#include "NetSink_test.moc"
