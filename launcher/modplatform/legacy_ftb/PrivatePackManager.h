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

#include <QFile>
#include <QSet>
#include <QString>

namespace LegacyFTB {

class PrivatePackManager {
   public:
    ~PrivatePackManager() { save(); }
    void load();
    void save() const;
    bool empty() const { return currentPacks.empty(); }
    const QSet<QString>& getCurrentPackCodes() const { return currentPacks; }
    void add(const QString& code)
    {
        currentPacks.insert(code);
        dirty = true;
    }
    void remove(const QString& code)
    {
        currentPacks.remove(code);
        dirty = true;
    }

   private:
    QSet<QString> currentPacks;
    QString m_filename = "private_packs.txt";
    mutable bool dirty = false;
};

}  // namespace LegacyFTB
