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
#pragma once
#include <QFuture>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QTcpSocket>

#include <Exception.h>

// Client for the Minecraft protocol
class McClient : public QObject
{
	Q_OBJECT

	QString m_domain;
	QString m_ip;
	short m_port;
	QTcpSocket m_socket;

	// 0: did not start reading the response yet
	// 1: read the response length, still reading the response
	// 2: finished reading the response
	unsigned m_responseReadState = 0;
	unsigned m_wantedRespLength	 = 0;
	QByteArray m_resp;

  public:
	explicit McClient(QObject* parent, QString domain, QString ip, short port);
	//! Read status data of the server, and calls the succeeded() signal with the parsed JSON data
	void getStatusData();

  private:
	void sendRequest();
	//! Accumulate data until we have a full response, then call parseResponse() once
	void readRawResponse();
	void parseResponse();

	void writeVarInt(QByteArray& data, int value);
	int readVarInt(QByteArray& data);
	char readByte(QByteArray& data);
	//! write number with specified size in big endian format
	void writeFixedInt(QByteArray& data, int value, int size);
	void writeString(QByteArray& data, const std::string& value);

	void writePacketToSocket(QByteArray& data);

	void emitFail(QString error);
	void emitSucceed(QJsonObject data);

  signals:
	void succeeded(QJsonObject data);
	void failed(QString error);
	void finished();
};
