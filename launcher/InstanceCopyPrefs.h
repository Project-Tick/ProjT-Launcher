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

#include <QStringList>

struct InstanceCopyPrefs
{
  public:
	bool allTrue() const;
	QString getSelectedFiltersAsRegex() const;
	QString getSelectedFiltersAsRegex(const QStringList& additionalFilters) const;
	// Getters
	bool isCopySavesEnabled() const;
	bool isKeepPlaytimeEnabled() const;
	bool isCopyGameOptionsEnabled() const;
	bool isCopyResourcePacksEnabled() const;
	bool isCopyShaderPacksEnabled() const;
	bool isCopyServersEnabled() const;
	bool isCopyModsEnabled() const;
	bool isCopyScreenshotsEnabled() const;
	bool isUseSymLinksEnabled() const;
	bool isLinkRecursivelyEnabled() const;
	bool isUseHardLinksEnabled() const;
	bool isDontLinkSavesEnabled() const;
	bool isUseCloneEnabled() const;
	// Setters
	void enableCopySaves(bool b);
	void enableKeepPlaytime(bool b);
	void enableCopyGameOptions(bool b);
	void enableCopyResourcePacks(bool b);
	void enableCopyShaderPacks(bool b);
	void enableCopyServers(bool b);
	void enableCopyMods(bool b);
	void enableCopyScreenshots(bool b);
	void enableUseSymLinks(bool b);
	void enableLinkRecursively(bool b);
	void enableUseHardLinks(bool b);
	void enableDontLinkSaves(bool b);
	void enableUseClone(bool b);

  protected: // data
	bool copySaves		   = true;
	bool keepPlaytime	   = true;
	bool copyGameOptions   = true;
	bool copyResourcePacks = true;
	bool copyShaderPacks   = true;
	bool copyServers	   = true;
	bool copyMods		   = true;
	bool copyScreenshots   = true;
	bool useSymLinks	   = false;
	bool linkRecursively   = false;
	bool useHardLinks	   = false;
	bool dontLinkSaves	   = false;
	bool useClone		   = false;
};
