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

#include <QCryptographicHash>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTest>

#include <net/ChecksumValidator.h>
#include <net/NetUtils.h>

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

class NetUtilsTest : public QObject
{
	Q_OBJECT

  private slots:
	void test_isApplicationError()
	{
		QVERIFY(Net::isApplicationError(QNetworkReply::ContentNotFoundError));
		QVERIFY(Net::isApplicationError(QNetworkReply::AuthenticationRequiredError));
		QVERIFY(Net::isApplicationError(QNetworkReply::InternalServerError));

		QVERIFY(!Net::isApplicationError(QNetworkReply::NoError));
		QVERIFY(!Net::isApplicationError(QNetworkReply::ConnectionRefusedError));
		QVERIFY(!Net::isApplicationError(QNetworkReply::TimeoutError));
	}

	void test_checksumValidator_matches()
	{
		QCryptographicHash hash(QCryptographicHash::Sha256);
		hash.addData(QByteArrayView("hello", 5));
		const QByteArray expected = hash.result();

		Net::ChecksumValidator validator(QCryptographicHash::Sha256, expected);
		QNetworkRequest request;
		QVERIFY(validator.init(request));
		QByteArray data("hello");
		QVERIFY(validator.write(data));

		DummyReply reply;
		QVERIFY(validator.validate(reply));
	}

	void test_checksumValidator_mismatch()
	{
		QCryptographicHash hash(QCryptographicHash::Sha256);
		hash.addData(QByteArrayView("world", 5));
		const QByteArray expected = hash.result();

		Net::ChecksumValidator validator(QCryptographicHash::Sha256, expected);
		QNetworkRequest request;
		QVERIFY(validator.init(request));
		QByteArray data("hello");
		QVERIFY(validator.write(data));

		DummyReply reply;
		QVERIFY(!validator.validate(reply));
	}
};

QTEST_GUILESS_MAIN(NetUtilsTest)

#include "NetUtils_test.moc"
