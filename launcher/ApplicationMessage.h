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
 *  This file does not contain upstream license blocks from prior projects.
 *  All content is either original Project Tick work or GPL-compatible.
 */

#pragma once

#include <QByteArray>
#include <QHash>
#include <QString>

struct ApplicationMessage {
    QString command;
    QHash<QString, QString> args;

    QByteArray serialize();
    void parse(const QByteArray& input);
};
