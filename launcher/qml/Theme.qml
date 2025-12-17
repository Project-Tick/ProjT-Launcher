// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

pragma Singleton
import QtQuick 2.15

QtObject {
    // Palette
    // Dark palette
    readonly property color background: "#0a0c10"
    readonly property color surface: "#12151a"
    readonly property color surfaceVariant: "#171c24"
    readonly property color textPrimary: "#dfe3e9"
    readonly property color textSecondary: "#9aa6b7"
    readonly property color accent: "#3c7be0"
    readonly property color danger: "#d55f5f"

    // Layout
    readonly property int radius: 8
    readonly property int spacingS: 8
    readonly property int spacingM: 12
    readonly property int spacingL: 16

    // Typography
    readonly property int fontHeader: 18
    readonly property int fontSubtitle: 14
    readonly property int fontBody: 12
    readonly property int fontCaption: 11
}
