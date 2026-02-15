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

#include <net/RawHeaderProxy.h>

class NetHeaderProxyTest : public QObject
{
	Q_OBJECT

  private slots:
	void test_rawHeaderProxy()
	{
		Net::RawHeaderProxy proxy;
		proxy.addHeader("X-Test", "1");
		proxy.addHeader("X-Other", "value");

		QNetworkRequest request(QUrl("https://example.com"));
		proxy.writeHeaders(request);

		QCOMPARE(request.rawHeader("X-Test"), QByteArray("1"));
		QCOMPARE(request.rawHeader("X-Other"), QByteArray("value"));
	}
};

QTEST_GUILESS_MAIN(NetHeaderProxyTest)

#include "NetHeaderProxy_test.moc"
