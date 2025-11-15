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

#include <QString>

namespace Override {

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

}  // namespace Override
