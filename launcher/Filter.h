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

#include <QRegularExpression>
#include <QString>

using Filter = std::function<bool(const QString&)>;

namespace Filters {
inline Filter inverse(Filter filter)
{
    return [filter = std::move(filter)](const QString& src) { return !filter(src); };
}

inline Filter any(QList<Filter> filters)
{
    return [filters = std::move(filters)](const QString& src) {
        for (auto& filter : filters)
            if (filter(src))
                return true;

        return false;
    };
}

inline Filter equals(QString pattern)
{
    return [pattern = std::move(pattern)](const QString& src) { return src == pattern; };
}

inline Filter equalsAny(QStringList patterns = {})
{
    return [patterns = std::move(patterns)](const QString& src) { return patterns.isEmpty() || patterns.contains(src); };
}

inline Filter equalsOrEmpty(QString pattern)
{
    return [pattern = std::move(pattern)](const QString& src) { return src.isEmpty() || src == pattern; };
}

inline Filter contains(QString pattern)
{
    return [pattern = std::move(pattern)](const QString& src) { return src.contains(pattern); };
}

inline Filter startsWith(QString pattern)
{
    return [pattern = std::move(pattern)](const QString& src) { return src.startsWith(pattern); };
}

inline Filter regexp(QRegularExpression pattern)
{
    return [pattern = std::move(pattern)](const QString& src) { return pattern.match(src).hasMatch(); };
}
}  // namespace Filters
