// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
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
 */
#pragma once

#include <QObject>
#include <QString>

#include <tasks/Task.h>

class ServerPingTask : public Task {
    Q_OBJECT
   public:
    explicit ServerPingTask(QString domain, int port) : Task(), m_domain(domain), m_port(port) {}
    ~ServerPingTask() override = default;
    int m_outputOnlinePlayers = -1;

   private:
    QString m_domain;
    int m_port;

   protected:
    virtual void executeTask() override;
};
