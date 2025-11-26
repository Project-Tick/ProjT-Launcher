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
 * Copyright 2013-2021 MultiMC Contributors
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
 * ======================================================================== */

#include "MinecraftTarget.h"

#include <QStringList>

// FIXME: the way this is written, it can't ever do any sort of validation and can accept total junk
MinecraftTarget MinecraftTarget::parse(const QString& fullAddress, bool useWorld)
{
    if (useWorld) {
        MinecraftTarget target;
        target.world = fullAddress;
        return target;
    }
    QStringList split = fullAddress.split(":");

    // The logic below replicates the exact logic minecraft uses for parsing server addresses.
    // While the conversion is not lossless and eats errors, it ensures the same behavior
    // within Minecraft and ProjT Launcher when entering server addresses.
    if (fullAddress.startsWith("[")) {
        int bracket = fullAddress.indexOf("]");
        if (bracket > 0) {
            QString ipv6 = fullAddress.mid(1, bracket - 1);
            QString port = fullAddress.mid(bracket + 1).trimmed();

            if (port.startsWith(":") && !ipv6.isEmpty()) {
                port = port.mid(1);
                split = QStringList({ ipv6, port });
            } else {
                split = QStringList({ ipv6 });
            }
        }
    }

    if (split.size() > 2) {
        split = QStringList({ fullAddress });
    }

    QString realAddress = split[0];

    quint16 realPort = 25565;
    if (split.size() > 1) {
        bool ok;
        realPort = split[1].toUInt(&ok);

        if (!ok) {
            realPort = 25565;
        }
    }

    return MinecraftTarget{ realAddress, realPort };
}
