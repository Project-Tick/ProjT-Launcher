// SPDX-License-Identifier: GPL-3.0-or-later AND Apache-2.0
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
 *
 * === Upstream License Block (Do Not Modify) ==============================
 *
 * Copyright 2021 Jamie Mansfield <jmansfield@cadixdev.org>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
    ======================================================================== */

#pragma once

#include <QString>

namespace Time {

QString prettifyDuration(int64_t duration, bool noDays = false);

/**
 * @brief Returns a string with short form time duration ie. `2days 1h3m4s56.0ms`.
 * miliseconds are only included if `precision` is greater than 0.
 *
 * @param duration a number of seconds as floating point
 * @param precision number of decmial points to display on fractons of a second, defualts to 0.
 * @return QString
 */
QString humanReadableDuration(double duration, int precision = 0);
}  // namespace Time
