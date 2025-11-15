// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  This file is part of ProjT Launcher and is licensed under
 *  the GNU General Public License version 3 or later.
 *
 *  If this file includes work from previous open-source projects,
 *  their original copyright and license notices are preserved below.
 *
 * === Upstream License Block (Do Not Modify) ==============================
 *
 * // SPDX-License-Identifier: GPL-3.0-only
 *
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (c) 2024 Leia uwu <leia@tutamail.com>
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
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * ======================================================================== */

#include "PrintServers.h"
#include "QHostInfo"

PrintServers::PrintServers(LaunchTask* parent, const QStringList& servers) : LaunchStep(parent)
{
    m_servers = servers;
}

void PrintServers::executeTask()
{
    for (QString server : m_servers) {
        QHostInfo::lookupHost(server, this, &PrintServers::resolveServer);
    }
}

void PrintServers::resolveServer(const QHostInfo& host_info)
{
    QString server = host_info.hostName();
    QString addresses = server + " resolves to:\n    [";

    if (!host_info.addresses().isEmpty()) {
        for (QHostAddress address : host_info.addresses()) {
            addresses += address.toString();
            if (!host_info.addresses().endsWith(address)) {
                addresses += ", ";
            }
        }
    } else {
        addresses += "N/A";
    }
    addresses += "]\n\n";

    m_server_to_address.insert(server, addresses);

    // print server info in order once all servers are resolved
    if (m_server_to_address.size() >= m_servers.size()) {
        for (QString serv : m_servers) {
            emit logLine(m_server_to_address.value(serv), MessageLevel::Launcher);
        }
        emitSucceeded();
    }
}

bool PrintServers::canAbort() const
{
    return true;
}
