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
