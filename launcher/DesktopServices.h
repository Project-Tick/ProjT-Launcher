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
#include <QUrl>

class QFileInfo;

/**
 * This wraps around QDesktopServices and adds workarounds where needed
 * Use this instead of QDesktopServices!
 */
namespace DesktopServices
{
	/**
	 * Open a path in whatever application is applicable.
	 * @param ensureFolderPathExists Make sure the path exists
	 */
	bool openPath(const QFileInfo& path, bool ensureFolderPathExists = false);

	/**
	 * Open a path in whatever application is applicable.
	 * @param ensureFolderPathExists Make sure the path exists
	 */
	bool openPath(const QString& path, bool ensureFolderPathExists = false);

	/**
	 * Run an application
	 */
	bool run(const QString& application,
			 const QStringList& args,
			 const QString& workingDirectory = QString(),
			 qint64* pid					 = 0);

	/**
	 * Open the URL, most likely in a browser. Maybe.
	 */
	bool openUrl(const QUrl& url);

	/**
	 * Determine whether the launcher is running in a Flatpak environment
	 */
	bool isFlatpak();

	/**
	 * Determine whether the launcher is running in a Snap environment
	 */
	bool isSnap();
} // namespace DesktopServices
