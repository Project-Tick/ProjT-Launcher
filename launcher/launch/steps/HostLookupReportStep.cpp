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

#include "HostLookupReportStep.hpp"

#include <QHostAddress>
#include <QHostInfo>

namespace projt::launch::steps
{
	HostLookupReportStep::HostLookupReportStep(projt::launch::LaunchPipeline* parent, QStringList hosts)
		: projt::launch::LaunchStage(parent),
		  m_hosts(hosts)
	{}

	void HostLookupReportStep::executeTask()
	{
		if (m_hosts.isEmpty())
		{
			emitSucceeded();
			return;
		}

		m_pending = m_hosts.size();
		for (const auto& host : m_hosts)
		{
			int lookupId = QHostInfo::lookupHost(host, this, &HostLookupReportStep::onLookupFinished);
			m_lookupById.insert(lookupId, host);
		}
	}

	void HostLookupReportStep::onLookupFinished(const QHostInfo& info)
	{
		if (isFinished())
		{
			return;
		}

		QString host = m_lookupById.value(info.lookupId(), info.hostName());
		m_messages.insert(host, formatMessage(host, info));
		m_pending--;
		finalizeIfReady();
	}

	QString HostLookupReportStep::formatMessage(const QString& host, const QHostInfo& info) const
	{
		QStringList addresses;
		for (const auto& address : info.addresses())
		{
			addresses.append(address.toString());
		}

		QString addressText = addresses.isEmpty() ? "N/A" : addresses.join(", ");
		QString errorSuffix;
		if (info.error() != QHostInfo::NoError && !info.errorString().isEmpty())
		{
			errorSuffix = QString(" (%1)").arg(info.errorString());
		}

		return QString("%1 resolves to:\n    [%2]%3\n\n").arg(host, addressText, errorSuffix);
	}

	void HostLookupReportStep::finalizeIfReady()
	{
		if (m_pending > 0)
		{
			return;
		}

		for (const auto& host : m_hosts)
		{
			emit logLine(m_messages.value(host), MessageLevel::Launcher);
		}
		emitSucceeded();
	}

	bool HostLookupReportStep::canAbort() const
	{
		return true;
	}
} // namespace projt::launch::steps
