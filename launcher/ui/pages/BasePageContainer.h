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

class BasePage;

class BasePageContainer {
   public:
    virtual ~BasePageContainer() {};
    virtual bool selectPage(QString pageId) = 0;
    virtual BasePage* selectedPage() const = 0;
    virtual BasePage* getPage(QString pageId) { return nullptr; };
    virtual void refreshContainer() = 0;
    virtual bool requestClose() = 0;
};
