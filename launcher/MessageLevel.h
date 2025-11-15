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

#include <qlogging.h>
#include <QString>

/**
 * @brief the MessageLevel Enum
 * defines what level a log message is
 */
namespace MessageLevel {
enum Enum {
    Unknown,  /**< No idea what this is or where it came from */
    StdOut,   /**< Undetermined stderr messages */
    StdErr,   /**< Undetermined stdout messages */
    Launcher, /**< Launcher Messages */
    Trace,    /**< Trace Messages */
    Debug,    /**< Debug Messages */
    Info,     /**< Info Messages */
    Message,  /**< Standard Messages */
    Warning,  /**< Warnings */
    Error,    /**< Errors */
    Fatal,    /**< Fatal Errors */
};
MessageLevel::Enum getLevel(const QString& levelName);
MessageLevel::Enum getLevel(QtMsgType type);

/* Get message level from a line. Line is modified if it was successful. */
MessageLevel::Enum fromLine(QString& line);

/* Get message level from a line from the launcher log. Line is modified if it was successful. */
MessageLevel::Enum fromLauncherLine(QString& line);
}  // namespace MessageLevel
