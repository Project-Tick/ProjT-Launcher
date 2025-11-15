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
#include "ParseUtils.h"
#include <QDateTime>
#include <QDebug>
#include <QString>
#include <cstdlib>

QDateTime timeFromS3Time(QString str)
{
    return QDateTime::fromString(str, Qt::ISODate);
}

QString timeToS3Time(QDateTime time)
{
    // this all because Qt can't format timestamps right.
    int offsetRaw = time.offsetFromUtc();
    bool negative = offsetRaw < 0;
    int offsetAbs = std::abs(offsetRaw);

    int offsetSeconds = offsetAbs % 60;
    offsetAbs -= offsetSeconds;

    int offsetMinutes = offsetAbs % 3600;
    offsetAbs -= offsetMinutes;
    offsetMinutes /= 60;

    int offsetHours = offsetAbs / 3600;

    QString raw = time.toString("yyyy-MM-ddTHH:mm:ss");
    raw += (negative ? QChar('-') : QChar('+'));
    raw += QString("%1").arg(offsetHours, 2, 10, QChar('0'));
    raw += ":";
    raw += QString("%1").arg(offsetMinutes, 2, 10, QChar('0'));
    return raw;
}
