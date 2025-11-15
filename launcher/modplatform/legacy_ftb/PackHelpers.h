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
#pragma once

#include <QList>
#include <QMetaType>
#include <QString>
#include <QStringList>

namespace LegacyFTB {

// Header for structs etc...
enum class PackType { Public, ThirdParty, Private };

struct Modpack {
    QString name;
    QString description;
    QString author;
    QStringList oldVersions;
    QString currentVersion;
    QString mcVersion;
    QString mods;
    QString logo;

    // Technical data
    QString dir;
    QString file;  //<- Url in the xml, but doesn't make much sense

    bool bugged = false;
    bool broken = false;

    PackType type;
    QString packCode;
};

using ModpackList = QList<Modpack>;

}  // namespace LegacyFTB

// We need it for the proxy model
Q_DECLARE_METATYPE(LegacyFTB::Modpack)
