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
