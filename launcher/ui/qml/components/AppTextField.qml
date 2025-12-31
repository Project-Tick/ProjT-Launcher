// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team

import QtQuick 2.15
import QtQuick.Controls 2.15
import ProjTLauncher 1.0

TextField {
    id: control

    // Config
    selectByMouse: true
    color: enabled ? ThemeColors.text : ThemeColors.textDisabled
    selectionColor: ThemeColors.accent
    selectedTextColor: ThemeColors.accentText
    placeholderTextColor: ThemeColors.textMuted
    font: ThemeColors.fontBody

    leftPadding: 10
    rightPadding: 10
    topPadding: 8
    bottomPadding: 8

    background: Rectangle {
        implicitWidth: 200
        implicitHeight: 36
        
        radius: ThemeColors.radiusM
        color: control.enabled ? ThemeColors.surface3 : ThemeColors.bg1
        
        border.width: 1
        border.color: {
            if (control.activeFocus) return ThemeColors.borderFocus;
            if (control.hovered) return ThemeColors.textMuted;
            return "transparent"; 
        }

        // Focus ring animation could be added here
        Behavior on border.color { ColorAnimation { duration: 100 } }
    }
}
