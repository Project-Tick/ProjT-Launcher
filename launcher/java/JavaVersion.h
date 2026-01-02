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
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#pragma once

#include <QString>

// NOTE: apparently the GNU C library pollutes the global namespace with these... undef them.
#ifdef major
#undef major
#endif
#ifdef minor
#undef minor
#endif

class JavaVersion {
    friend class JavaVersionTest;

   public:
    JavaVersion() {}
    JavaVersion(const QString& rhs);
    JavaVersion(int major, int minor, int security, int build = 0, QString name = "");

    JavaVersion& operator=(const QString& rhs);

    bool operator<(const JavaVersion& rhs) const;
    bool operator==(const JavaVersion& rhs) const;
    bool operator>(const JavaVersion& rhs) const;

    bool requiresPermGen() const;
    bool defaultsToUtf8() const;
    bool isModular() const;

    QString toString() const;

    int major() const { return m_major; }
    int minor() const { return m_minor; }
    int security() const { return m_security; }
    QString build() const { return m_prerelease; }
    QString name() const { return m_name; }

   private:
    QString m_string;
    int m_major = 0;
    int m_minor = 0;
    int m_security = 0;
    QString m_name = "";
    bool m_parseable = false;
    QString m_prerelease;
};
