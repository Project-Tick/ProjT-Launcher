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
#include <QDateTime>
#include <QMap>
#include <QSet>
#include <QString>

struct FMLlib
{
	QString filename;
	QString checksum;
};

struct VersionFilterData
{
	VersionFilterData();
	// mapping between minecraft versions and FML libraries required
	QMap<QString, QList<FMLlib>> fmlLibsMapping;
	// set of minecraft versions for which using forge installers is blacklisted
	QSet<QString> forgeInstallerBlacklist;
	// no new versions below this date will be accepted from Mojang servers
	QDateTime legacyCutoffDate;
	// Libraries that belong to LWJGL
	QSet<QString> lwjglWhitelist;
	// release date of first version to require Java 8 (17w13a)
	QDateTime java8BeginsDate;
	// release data of first version to require Java 16 (21w19a)
	QDateTime java16BeginsDate;
	// release data of first version to require Java 17 (1.18 Pre Release 2)
	QDateTime java17BeginsDate;
};
extern VersionFilterData g_VersionFilterData;
