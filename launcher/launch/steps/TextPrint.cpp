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
#include "TextPrint.h"

TextPrint::TextPrint(LaunchTask* parent, const QStringList& lines, MessageLevel::Enum level) : LaunchStep(parent)
{
    m_lines = lines;
    m_level = level;
}
TextPrint::TextPrint(LaunchTask* parent, const QString& line, MessageLevel::Enum level) : LaunchStep(parent)
{
    m_lines.append(line);
    m_level = level;
}

void TextPrint::executeTask()
{
    emit logLines(m_lines, m_level);
    emitSucceeded();
}

bool TextPrint::canAbort() const
{
    return true;
}

bool TextPrint::abort()
{
    emitFailed("Aborted.");
    return true;
}
