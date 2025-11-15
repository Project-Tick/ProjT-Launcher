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
 *
 */
#pragma once

#include <QFileInfo>
#include <QWidget>
#include <optional>

namespace GuiUtil {
std::optional<QString> uploadPaste(const QString& name, const QFileInfo& filePath, QWidget* parentWidget);
std::optional<QString> uploadPaste(const QString& name, const QString& data, QWidget* parentWidget);
void setClipboardText(QString text);
QStringList BrowseForFiles(QString context, QString caption, QString filter, QString defaultPath, QWidget* parentWidget);
QString BrowseForFile(QString context, QString caption, QString filter, QString defaultPath, QWidget* parentWidget);
}  // namespace GuiUtil
