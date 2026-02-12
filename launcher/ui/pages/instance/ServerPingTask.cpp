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
#include <QFutureWatcher>

#include <Json.h>
#include "McClient.h"
#include "McResolver.h"
#include "ServerPingTask.h"

unsigned getOnlinePlayers(QJsonObject data)
{
	return Json::requireInteger(Json::requireObject(data, "players"), "online");
}

void ServerPingTask::executeTask()
{
	qDebug() << "Querying status of " << QString("%1:%2").arg(m_domain).arg(m_port);

	// Resolve the actual IP and port for the server
	McResolver* resolver = new McResolver(nullptr, m_domain, m_port);
	connect(resolver,
			&McResolver::succeeded,
			this,
			[this](QString ip, int port)
			{
				qDebug() << "Resolved Address for" << m_domain << ": " << ip << ":" << port;

				// Now that we have the IP and port, query the server
				McClient* client = new McClient(nullptr, m_domain, ip, port);

				connect(client,
						&McClient::succeeded,
						this,
						[this](QJsonObject data)
						{
							m_outputOnlinePlayers = getOnlinePlayers(data);
							qDebug() << "Online players: " << m_outputOnlinePlayers;
							emitSucceeded();
						});
				connect(client, &McClient::failed, this, [this](QString error) { emitFailed(error); });

				// Delete McClient object when done
				connect(client, &McClient::finished, this, [client]() { client->deleteLater(); });
				client->getStatusData();
			});
	connect(resolver, &McResolver::failed, this, [this](QString error) { emitFailed(error); });

	// Delete McResolver object when done
	connect(resolver, &McResolver::finished, [resolver]() { resolver->deleteLater(); });
	resolver->ping();
}