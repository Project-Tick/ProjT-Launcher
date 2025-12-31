// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team

import QtQuick 2.15
import QtQuick.Controls 2.15
import ProjTLauncher 1.0

// MACOS NATIVE STYLE OVERRIDE
TextField {
    id: control

    // Config
    selectByMouse: true
    // macOS Text Color: normally black/white.
    color: enabled ? (ThemeColors.isDark ? "#DFDFDF" : "#000000") : (ThemeColors.isDark ? "#666666" : "#999999")
    selectionColor: "#007AFF" // System Blue
    selectedTextColor: "#FFFFFF"
    placeholderTextColor: ThemeColors.isDark ? "#777777" : "#999999"
    
    // SF Pro Text emulation
    font.family: "SF Pro Text"
    font.pixelSize: 13
    
    leftPadding: 6
    rightPadding: 6
    topPadding: 5
    bottomPadding: 5

    background: Rectangle {
        implicitWidth: 200
        implicitHeight: 22 
        
        radius: 5
        color: control.enabled ? (ThemeColors.isDark ? "#1E1E1E" : "#FFFFFF") : (ThemeColors.isDark ? "#2C2C2C" : "#F2F2F2")
        
        border.width: 1
        border.color: {
            if (control.activeFocus) return "#007AFF"
            return ThemeColors.isDark ? "#555555" : "#D1D1D1"
        }

        // macOS Focus Halo
        Rectangle {
            anchors.fill: parent
            anchors.margins: -3
            radius: parent.radius + 3
            color: "transparent"
            border.width: 2
            border.color: "#80007AFF" // Transparent Blue
            visible: control.activeFocus
        }
    }
}
