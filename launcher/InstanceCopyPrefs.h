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

#include <QStringList>

struct InstanceCopyPrefs {
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

   protected:  // data
    bool copySaves = true;
    bool keepPlaytime = true;
    bool copyGameOptions = true;
    bool copyResourcePacks = true;
    bool copyShaderPacks = true;
    bool copyServers = true;
    bool copyMods = true;
    bool copyScreenshots = true;
    bool useSymLinks = false;
    bool linkRecursively = false;
    bool useHardLinks = false;
    bool dontLinkSaves = false;
    bool useClone = false;
};
