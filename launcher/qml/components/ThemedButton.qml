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

import QtQuick 2.15
import QtQuick.Controls 2.15
import ProjTLauncher 1.0

Button {
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
    property color _highlightColor: {
        var _ = _themeUpdateCount;
        return themeVM ? themeVM.highlightColor : ThemeColors.highlight;
    }
    property color _highlightedTextColor: {
        var _ = _themeUpdateCount;
        return themeVM ? themeVM.highlightedTextColor : ThemeColors.highlightedText;
    }
    property color _buttonColor: {
        var _ = _themeUpdateCount;
        return themeVM ? themeVM.buttonColor : ThemeColors.button;
    }
    property color _buttonTextColor: {
        var _ = _themeUpdateCount;
        return themeVM ? themeVM.buttonTextColor : ThemeColors.buttonText;
    }
    property color _textColor: {
        var _ = _themeUpdateCount;
        return themeVM ? themeVM.textColor : ThemeColors.text;
    }
    property color _disabledColor: {
        var _ = _themeUpdateCount;
        return themeVM ? Qt.rgba(themeVM.textColor.r, themeVM.textColor.g, themeVM.textColor.b, 0.3) : ThemeColors.disabled;
    }
    property color _errorColor: ThemeColors.error
    property color _successColor: ThemeColors.success
    property color _warningColor: ThemeColors.warning

    // Button variants
    property bool primary: false
    property bool danger: false
    property bool success: false
    property bool warning: false
    property bool outline: false
    property bool flatStyle: false  // Renamed from 'flat' to avoid FINAL property conflict
    property bool rounded: false

    // Size variants
    property string size: "medium" // "small", "medium", "large"

    implicitHeight: {
        switch (size) {
        case "small":
            return 28;
        case "large":
            return 40;
        default:
            return 34;
        }
    }

    implicitWidth: Math.max(implicitHeight, contentItem.implicitWidth + leftPadding + rightPadding)

    leftPadding: size === "small" ? 10 : 14
    rightPadding: leftPadding

    font.pixelSize: {
        switch (size) {
        case "small":
            return 12;
        case "large":
            return 15;
        default:
            return 13;
        }
    }
    font.weight: Font.Medium

    // Determine button color based on variant
    readonly property color baseColor: {
        var _ = _themeUpdateCount;
        if (control.danger)
            return _errorColor;
        if (control.success)
            return _successColor;
        if (control.warning)
            return _warningColor;
        if (control.primary)
            return _highlightColor;
        return _buttonColor;
    }

    readonly property color textColorComputed: {
        var _ = _themeUpdateCount;
        if (!control.enabled)
            return _disabledColor;
        if (control.outline || control.flatStyle) {
            if (control.danger)
                return _errorColor;
            if (control.success)
                return _successColor;
            if (control.warning)
                return _warningColor;
            if (control.primary)
                return _highlightColor;
            return _textColor;
        }
        if (control.primary || control.danger || control.success || control.warning) {
            return _highlightedTextColor;
        }
        return _buttonTextColor;
    }

    contentItem: Row {
        spacing: control.icon.name ? 6 : 0
        anchors.centerIn: parent

        // Icon support via Qt's built-in icon property
        Image {
            visible: control.icon.name.length > 0 || control.icon.source.toString().length > 0
            source: control.icon.source
            width: control.icon.width > 0 ? control.icon.width : 16
            height: control.icon.height > 0 ? control.icon.height : 16
            anchors.verticalCenter: parent.verticalCenter

            // Color overlay for monochrome icons
            layer.enabled: true
            layer.effect: Item {}
        }

        // Button text
        Text {
            text: control.text
            font: control.font
            color: control.textColorComputed
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    background: Rectangle {
        id: bgRect
        implicitWidth: 100
        implicitHeight: control.implicitHeight
        radius: control.rounded ? height / 2 : ThemeColors.radiusS
        opacity: enabled ? 1.0 : 0.5

        // Background color with gradient effect
        color: {
            var _ = control._themeUpdateCount;
            if (control.flatStyle)
                return control.hovered ? Qt.rgba(control.baseColor.r, control.baseColor.g, control.baseColor.b, 0.1) : "transparent";
            if (control.outline)
                return control.hovered ? Qt.rgba(control.baseColor.r, control.baseColor.g, control.baseColor.b, 0.1) : "transparent";

            var c = control.baseColor;
            if (control.pressed)
                return Qt.darker(c, 1.2);
            if (control.hovered)
                return Qt.lighter(c, 1.1);
            return c;
        }

        border.width: {
            if (control.outline)
                return 1.5;
            if (control.visualFocus)
                return 2;
            if (control.flatStyle)
                return 0;
            return 1;
        }

        border.color: {
            var _ = control._themeUpdateCount;
            if (control.visualFocus)
                return control._highlightColor;
            if (control.outline) {
                if (control.hovered)
                    return Qt.lighter(control.baseColor, 1.2);
                return control.baseColor;
            }
            if (control.flatStyle)
                return "transparent";
            return Qt.darker(control.baseColor, 1.2);
        }

        // Smooth color transitions
        Behavior on color {
            ColorAnimation {
                duration: 150
                easing.type: Easing.OutQuad
            }
        }
        Behavior on border.color {
            ColorAnimation {
                duration: 150
            }
        }
    }

    // Subtle scale animation on press
    scale: control.pressed ? 0.97 : 1.0
    Behavior on scale {
        NumberAnimation {
            duration: 80
            easing.type: Easing.OutQuad
        }
    }

    // Cursor change on hover
    MouseArea {
        anchors.fill: parent
        cursorShape: control.enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
        acceptedButtons: Qt.NoButton
    }
}
