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

#include "McResolver.h"

McResolver::McResolver(QObject* parent, QString domain, int port)
	: QObject(parent),
	  m_constrDomain(domain),
	  m_constrPort(port)
{}

void McResolver::ping()
{
	pingWithDomainSRV(m_constrDomain, m_constrPort);
}

void McResolver::pingWithDomainSRV(QString domain, int port)
{
	QDnsLookup* lookup = new QDnsLookup(this);
	lookup->setName(QString("_minecraft._tcp.%1").arg(domain));
	lookup->setType(QDnsLookup::SRV);

	connect(lookup,
			&QDnsLookup::finished,
			this,
			[this, domain, port]()
			{
				QDnsLookup* lookup = qobject_cast<QDnsLookup*>(sender());

				lookup->deleteLater();

				if (lookup->error() != QDnsLookup::NoError)
				{
					qDebug() << QString("Warning: SRV record lookup failed (%1), trying A record lookup")
									.arg(lookup->errorString());
					pingWithDomainA(domain, port);
					return;
				}

				auto records = lookup->serviceRecords();
				if (records.isEmpty())
				{
					qDebug() << "Warning: no SRV entries found for domain, trying A record lookup";
					pingWithDomainA(domain, port);
					return;
				}

				const auto& firstRecord = records.at(0);
				QString newDomain		= firstRecord.target();
				int newPort				= firstRecord.port();
				pingWithDomainA(newDomain, newPort);
			});

	lookup->lookup();
}

void McResolver::pingWithDomainA(QString domain, int port)
{
	QHostInfo::lookupHost(domain,
						  this,
						  [this, port](const QHostInfo& hostInfo)
						  {
							  if (hostInfo.error() != QHostInfo::NoError)
							  {
								  emitFail("A record lookup failed");
								  return;
							  }

							  auto records = hostInfo.addresses();
							  if (records.isEmpty())
							  {
								  emitFail("No A entries found for domain");
								  return;
							  }

							  const auto& firstRecord = records.at(0);
							  emitSucceed(firstRecord.toString(), port);
						  });
}

void McResolver::emitFail(QString error)
{
	qDebug() << "DNS resolver error:" << error;
	emit failed(error);
	emit finished();
}

void McResolver::emitSucceed(QString ip, int port)
{
	emit succeeded(ip, port);
	emit finished();
}
