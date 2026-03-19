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

#include "ServerJoinResolveStep.hpp"

#include <launch/LaunchPipeline.hpp>

#include <QDnsLookup>

namespace projt::launch::steps
{
	ServerJoinResolveStep::ServerJoinResolveStep(projt::launch::LaunchPipeline* parent)
		: projt::launch::LaunchStage(parent)
	{
		auto lookup = new QDnsLookup(this);
		lookup->setType(QDnsLookup::SRV);
		connect(lookup, &QDnsLookup::finished, this, &ServerJoinResolveStep::onLookupFinished);
		m_lookup = lookup;
	}

	void ServerJoinResolveStep::setLookupAddress(const QString& address)
	{
		m_lookupAddress = address;
		m_queryName		= QString("_minecraft._tcp.%1").arg(address);
		if (m_lookup)
		{
			m_lookup->setName(m_queryName);
		}
	}

	void ServerJoinResolveStep::setOutputTarget(MinecraftTarget::Ptr target)
	{
		m_output = target;
	}

	void ServerJoinResolveStep::executeTask()
	{
		if (!m_lookup)
		{
			emitFailed(QString("DNS lookup failed to initialize."));
			return;
		}
		if (m_lookupAddress.isEmpty())
		{
			emitFailed(QString("Server address is empty."));
			return;
		}
		if (!m_output)
		{
			emitFailed(QString("Server target is missing."));
			return;
		}
		m_lookup->lookup();
	}

	bool ServerJoinResolveStep::abort()
	{
		if (m_lookup)
		{
			m_lookup->abort();
		}
		emitAborted();
		return true;
	}

	void ServerJoinResolveStep::onLookupFinished()
	{
		if (isFinished())
		{
			return;
		}
		if (!m_lookup)
		{
			emitFailed(QString("DNS lookup failed to initialize."));
			return;
		}

		if (m_lookup->error() != QDnsLookup::NoError)
		{
			emit logLine(QString("Failed to resolve server address (this is NOT an error!) %1: %2\n")
							 .arg(m_lookup->name(), m_lookup->errorString()),
						 MessageLevel::Launcher);
			finalizeTarget(m_lookupAddress, 25565);
			return;
		}

		const auto records = m_lookup->serviceRecords();
		if (records.empty())
		{
			emit logLine(
				QString(
					"Failed to resolve server address %1: the DNS lookup succeeded, but no records were returned.\n")
					.arg(m_lookup->name()),
				MessageLevel::Warning);
			finalizeTarget(m_lookupAddress, 25565);
			return;
		}

		const auto& firstRecord = records.at(0);
		quint16 port			= firstRecord.port();

		emit logLine(QString("Resolved server address %1 to %2 with port %3\n")
						 .arg(m_lookup->name(), firstRecord.target(), QString::number(port)),
					 MessageLevel::Launcher);
		finalizeTarget(firstRecord.target(), port);
	}

	void ServerJoinResolveStep::finalizeTarget(const QString& address, quint16 port)
	{
		m_output->address = address;
		m_output->port	  = port;

		emitSucceeded();
	}
} // namespace projt::launch::steps
