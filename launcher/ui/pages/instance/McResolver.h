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
#include <QtNetwork/qtcpsocket.h>
#include <QDnsLookup>
#include <QHostInfo>
#include <QObject>
#include <QString>

// resolve the IP and port of a Minecraft server
class McResolver : public QObject
{
	Q_OBJECT

	QString m_constrDomain;
	int m_constrPort;

  public:
	explicit McResolver(QObject* parent, QString domain, int port);
	void ping();

  private:
	void pingWithDomainSRV(QString domain, int port);
	void pingWithDomainA(QString domain, int port);
	void emitFail(QString error);
	void emitSucceed(QString ip, int port);

  signals:
	void succeeded(QString ip, int port);
	void failed(QString error);
	void finished();
};
