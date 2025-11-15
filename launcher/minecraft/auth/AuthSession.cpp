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
 */
#include "AuthSession.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringList>

QString AuthSession::serializeUserProperties()
{
    QJsonObject userAttrs;
    /*
    for (auto key : u.properties.keys())
    {
        auto array = QJsonArray::fromStringList(u.properties.values(key));
        userAttrs.insert(key, array);
    }
    */
    QJsonDocument value(userAttrs);
    return value.toJson(QJsonDocument::Compact);
}

bool AuthSession::MakeOffline(QString offline_playername)
{
    if (status != PlayableOffline && status != PlayableOnline) {
        return false;
    }
    session = "-";
    access_token = "0";
    player_name = offline_playername;
    status = PlayableOffline;
    return true;
}

void AuthSession::MakeDemo(QString name, QString u)
{
    wants_online = false;
    demo = true;
    uuid = u;
    session = "-";
    access_token = "0";
    player_name = name;
    status = PlayableOnline;  // needs online to download the assets
};