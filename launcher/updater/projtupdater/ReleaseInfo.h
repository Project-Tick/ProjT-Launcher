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
 *
 */

#pragma once
#include <QDateTime>
#include <QList>
#include <QString>
#include <QUrl>

#include <QDebug>

#include "Version.h"

struct ReleaseAsset
{
	QString name;
	QString label;
	QString content_type;
	int size = 0;
	QDateTime created_at;
	QDateTime updated_at;
	QUrl download_url;

	bool isValid() const
	{
		return !name.isEmpty() && download_url.isValid();
	}
};

struct ReleaseInfo
{
	QString name;
	QString tag_name;
	QDateTime created_at;
	QDateTime published_at;
	bool prerelease = false;
	bool draft		= false;
	QString body;
	QList<ReleaseAsset> assets;
	Version version;

	bool isValid() const
	{
		return !tag_name.isEmpty();
	}
};

QDebug operator<<(QDebug debug, const ReleaseAsset& releaseAsset);
QDebug operator<<(QDebug debug, const ReleaseInfo& releaseInfo);
