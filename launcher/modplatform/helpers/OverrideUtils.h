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

#include <QString>

namespace Override
{

	/** This creates a file in `parent_folder` that holds information about which
	 *  overrides are in `override_path`.
	 *
	 *  If there's already an existing such file, it will be ovewritten.
	 */
	void createOverrides(const QString& name, const QString& parent_folder, const QString& override_path);

	/** This reads an existing overrides archive, returning a list of overrides.
	 *
	 *  If there's no such file in `parent_folder`, it will return an empty list.
	 */
	QStringList readOverrides(const QString& name, const QString& parent_folder);

} // namespace Override
