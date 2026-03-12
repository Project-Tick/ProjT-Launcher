// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2026 Project Tick
 *  Copyright (C) 2023 Rachel Powers <508861+Ryex@users.noreply.github.com>
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

#include "ReleaseInfo.h"

QDebug operator<<(QDebug debug, const ReleaseAsset& asset)
{
	QDebugStateSaver saver(debug);
	debug.nospace() << "ReleaseAsset( "
					   "name: "
					<< asset.name
					<< ", "
					   "label: "
					<< asset.label
					<< ", "
					   "content_type: "
					<< asset.content_type
					<< ", "
					   "size: "
					<< asset.size
					<< ", "
					   "created_at: "
					<< asset.created_at
					<< ", "
					   "updated_at: "
					<< asset.updated_at
					<< ", "
					   "download_url: "
					<< asset.download_url
					<< " "
					   ")";
	return debug;
}

QDebug operator<<(QDebug debug, const ReleaseInfo& release)
{
	QDebugStateSaver saver(debug);
	debug.nospace() << "ReleaseInfo( "
					   "name: "
					<< release.name
					<< ", "
					   "tag_name: "
					<< release.tag_name
					<< ", "
					   "created_at: "
					<< release.created_at
					<< ", "
					   "published_at: "
					<< release.published_at
					<< ", "
					   "prerelease: "
					<< release.prerelease
					<< ", "
					   "draft: "
					<< release.draft
					<< ", "
					   "version: "
					<< release.version
					<< ", "
					   "body: "
					<< release.body
					<< ", "
					   "assets: "
					<< release.assets
					<< " "
					   ")";
	return debug;
}
