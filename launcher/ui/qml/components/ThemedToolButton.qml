// SPDX-License-Identifier: GPL-3.0-only
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
 *
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import ProjTLauncher 1.0

ToolButton {
    id: control

    // Theme binding - directly from themeVM for reliable updates
    property var themeVM: ProjT.themeVM
    property int _themeUpdateCount: 0

    // Listen for theme changes
    Connections {
        target: themeVM
        function onThemeColorsChanged() {
            control._themeUpdateCount++;
        }
    }

    // Computed theme colors
    property color _textColor: {
        var _ = _themeUpdateCount;
        return themeVM ? themeVM.textColor : ThemeColors.text;
    }
    property color _highlightColor: {
        var _ = _themeUpdateCount;
        return themeVM ? themeVM.highlightColor : ThemeColors.highlight;
    }
    property color _buttonColor: {
        var _ = _themeUpdateCount;
        return themeVM ? themeVM.buttonColor : ThemeColors.button;
    }
    property color _disabledColor: {
        var _ = _themeUpdateCount;
        return themeVM ? Qt.rgba(themeVM.textColor.r, themeVM.textColor.g, themeVM.textColor.b, 0.3) : ThemeColors.disabled;
    }
    property color _errorColor: ThemeColors.error
    property color _successColor: ThemeColors.success

    // Button variants
    property bool danger: false
    property bool success: false
    property bool active: false

    // Custom text color override
    property color customTextColor: "transparent"

    display: AbstractButton.TextBesideIcon

    implicitHeight: 30
    implicitWidth: Math.max(implicitHeight, contentItem.implicitWidth + leftPadding + rightPadding)

    scale: control.pressed ? 0.96 : 1.0
    Behavior on scale { NumberAnimation { duration: 50 } }

    leftPadding: 8
    rightPadding: 8

    font.pixelSize: 12
    font.weight: Font.Normal

    readonly property color effectiveTextColor: {
        var _ = _themeUpdateCount;
        if (customTextColor != "transparent" && customTextColor.a > 0)
            return customTextColor;
        if (!control.enabled)
            return _disabledColor;
        if (control.danger)
            return _errorColor;
        if (control.success)
            return _successColor;
        if (control.active || control.checked)
            return _highlightColor;
        return _textColor;
    }

    contentItem: Row {
        spacing: control.display === AbstractButton.TextBesideIcon ? 6 : 0
        anchors.centerIn: parent

        // Icon
        Image {
            visible: control.icon.source.toString().length > 0
            source: control.icon.source
            width: control.icon.width > 0 ? control.icon.width : 16
            height: control.icon.height > 0 ? control.icon.height : 16
            anchors.verticalCenter: parent.verticalCenter
        }

        // Text
        Text {
            visible: control.display !== AbstractButton.IconOnly && control.text.length > 0
            text: control.text
            font: control.font
            color: control.effectiveTextColor
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    background: Rectangle {
        implicitWidth: 32
        implicitHeight: 30
        radius: ThemeColors.radiusS

        color: {
            var _ = control._themeUpdateCount;
            var hoverColor = Qt.rgba(255, 255, 255, 0.04);
            var pressedColor = Qt.rgba(255, 255, 255, 0.08);

            if (control.checked || control.active) {
                return Qt.rgba(ThemeColors.accent.r, ThemeColors.accent.g, ThemeColors.accent.b, 0.12);
            }
            if (control.pressed)
                return pressedColor;
            if (control.hovered)
                return hoverColor;
            return "transparent";
        }

        border.width: (control.checked || control.active) ? 1 : 0
        border.color: ThemeColors.accent
        opacity: control.enabled ? ((control.checked || control.active) ? 1.0 : (control.hovered ? 0.8 : 0.0)) : 0.4

        Behavior on color {
            ColorAnimation {
                duration: ThemeColors.durationShort
            }
        }
        Behavior on opacity {
            NumberAnimation {
                duration: ThemeColors.durationShort
            }
        }
    }

    // Cursor
    MouseArea {
        anchors.fill: parent
        cursorShape: control.enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
        acceptedButtons: Qt.NoButton
    }
}
