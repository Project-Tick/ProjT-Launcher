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

template <typename T>
class DefaultVariable {
   public:
    DefaultVariable(const T& value) { defaultValue = value; }
    DefaultVariable<T>& operator=(const T& value)
    {
        currentValue = value;
        is_default = currentValue == defaultValue;
        is_explicit = true;
        return *this;
    }
    operator const T&() const { return is_default ? defaultValue : currentValue; }
    bool isDefault() const { return is_default; }
    bool isExplicit() const { return is_explicit; }

   private:
    T currentValue;
    T defaultValue;
    bool is_default = true;
    bool is_explicit = false;
};
