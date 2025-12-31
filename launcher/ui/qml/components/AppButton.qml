// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import ProjTLauncher 1.0

Button {
    id: control

    // Properties
    property string variant: "secondary" // "primary", "secondary", "ghost", "danger"
    property string iconSource: ""
    property bool loading: false

    // Size props
    property string size: "medium" // "small", "medium", "large"
    
    // Logic
    property bool isPrimary: variant === "primary"
    property bool isSecondary: variant === "secondary"
    property bool isGhost: variant === "ghost"
    property bool isDanger: variant === "danger"

    // Configuration
    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: size === "small" ? 28 : (size === "large" ? 44 : 36)

    padding: size === "small" ? 8 : 12
    horizontalPadding: padding + 4
    spacing: 8

    // Content Style
    contentItem: RowLayout {
        spacing:control.spacing
        
        // Icon
        Image {
            source: control.iconSource
            visible: control.iconSource !== "" && !control.loading
            Layout.preferredWidth: size === "small" ? 14 : 16
            Layout.preferredHeight: size === "small" ? 14 : 16
            sourceSize: Qt.size(Layout.preferredWidth, Layout.preferredHeight)
            fillMode: Image.PreserveAspectFit
            Layout.alignment: Qt.AlignVCenter
            
            // Tint icon if primary or danger
            /*ColorOverlay {
               source: parent
               color: label.color
               anchors.fill: parent
            }*/
        }
        
        // Loading Spinner (Placeholder)
        /*BusyIndicator {
            visible: control.loading
            Layout.preferredWidth: 16
            Layout.preferredHeight: 16
        }*/

        // Text
        Label {
            id: label
            text: control.text
            font: control.size === "large" ? ThemeColors.fontTitle : (control.size === "small" ? ThemeColors.fontCaption : ThemeColors.fontBodyBold)
            color: {
                if (!control.enabled) return ThemeColors.textDisabled;
                if (control.isPrimary) return ThemeColors.accentText;
                if (control.isDanger) return "#FFFFFF";
                if (control.isGhost) return control.pressed ? ThemeColors.text : ThemeColors.textSecondary;
                return ThemeColors.text; // Secondary default
            }
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            Layout.fillWidth: true
            elide: Text.ElideRight
        }
    }

    // Background Style
    background: Rectangle {
        implicitWidth: 100
        implicitHeight: control.implicitHeight
        
        radius: ThemeColors.radiusM
        
        color: {
            if (!control.enabled) return control.isGhost ? "transparent" : ThemeColors.surface3;
            
            if (control.isPrimary) {
                return control.pressed ? ThemeColors.accentHover : (control.hovered ? Qt.lighter(ThemeColors.accent, 1.1) : ThemeColors.accent);
            }
            if (control.isDanger) {
                return control.pressed ? ThemeColors.dangerHover : (control.hovered ? Qt.lighter(ThemeColors.danger, 1.1) : ThemeColors.danger);
            }
            if (control.isSecondary) {
                return control.pressed ? ThemeColors.surface2 : (control.hovered ? ThemeColors.surface2 : ThemeColors.surface);
            }
            // Ghost
            return control.pressed ? ThemeColors.pressedOverlay : (control.hovered ? ThemeColors.hoverOverlay : "transparent");
        }

        border.width: control.isSecondary && !control.pressed ? 1 : 0
        border.color: {
            if (!control.enabled) return "transparent";
            if (control.focus) return ThemeColors.borderFocus;
            return control.isSecondary ? ThemeColors.border : "transparent";
        }
        
        // Focus Ring
        Rectangle {
            anchors.fill: parent
            anchors.margins: -2
            radius: parent.radius + 2
            color: "transparent"
            border.width: 2
            border.color: ThemeColors.borderFocus
            visible: control.activeFocus
        }
    }

    // Cursor
    MouseArea {
        anchors.fill: parent
        cursorShape: control.enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
        acceptedButtons: Qt.NoButton // Pass through
    }
}
