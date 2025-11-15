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
inline void clamp(T& current, T min, T max)
{
    if (current < min) {
        current = min;
    } else if (current > max) {
        current = max;
    }
}

// List of numbers from min to max. Next is exponent times bigger than previous.

class ExponentialSeries {
   public:
    ExponentialSeries(unsigned min, unsigned max, unsigned exponent = 2)
    {
        m_current = m_min = min;
        m_max = max;
        m_exponent = exponent;
    }
    void reset() { m_current = m_min; }
    unsigned operator()()
    {
        unsigned retval = m_current;
        m_current *= m_exponent;
        clamp(m_current, m_min, m_max);
        return retval;
    }
    unsigned m_current;
    unsigned m_min;
    unsigned m_max;
    unsigned m_exponent;
};
