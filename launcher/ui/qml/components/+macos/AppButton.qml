// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import ProjTLauncher 1.0

// MACOS NATIVE STYLE OVERRIDE
Button {
    id: control

    // Properties
    property string variant: "secondary" // "primary", "secondary", "ghost", "danger"
    property string iconSource: ""
    property bool loading: false
    property string size: "medium" // "small", "medium", "large"
    
    property bool isPrimary: variant === "primary"
    property bool isSecondary: variant === "secondary"
    property bool isGhost: variant === "ghost"
    property bool isDanger: variant === "danger"

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: size === "small" ? 22 : (size === "large" ? 40 : 32) // Slightly shorter standard macOS buttons

    padding: size === "small" ? 4 : 10
    horizontalPadding: padding + 6
    spacing: 6

    contentItem: RowLayout {
        spacing: control.spacing
        
        Image {
            source: control.iconSource
            visible: control.iconSource !== "" && !control.loading
            Layout.preferredWidth: size === "small" ? 12 : 16
            Layout.preferredHeight: size === "small" ? 12 : 16
            sourceSize: Qt.size(Layout.preferredWidth, Layout.preferredHeight)
            fillMode: Image.PreserveAspectFit
            Layout.alignment: Qt.AlignVCenter
            opacity: control.enabled ? 1.0 : 0.5
        }
        
        Label {
            id: label
            text: control.text
            // Use native system font feel (San Francisco)
            font.family: "SF Pro Text" 
            font.weight: Font.Medium
            font.pixelSize: size === "small" ? 11 : 13
            
            color: {
                if (!control.enabled) return ThemeColors.textDisabled;
                if (control.isPrimary) return "#FFFFFF"; // Primary always white text on Mac
                if (control.isDanger) return "#FFFFFF";
                return ThemeColors.text; // Black/White based on theme
            }
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            Layout.fillWidth: true
            elide: Text.ElideRight
        }
    }

    background: Rectangle {
        implicitWidth: 100
        implicitHeight: control.implicitHeight
        
        radius: 6 // macOS style rounded rect (not full pill)
        
        // Native-like Gradient for Secondary/Standard buttons
        gradient: (control.isSecondary && !control.pressed && !control.isGhost) ? Gradient {
            GradientStop { position: 0.0; color: ThemeColors.isDark ? Qt.rgba(1,1,1,0.15) : Qt.rgba(1,1,1,1) }
            GradientStop { position: 1.0; color: ThemeColors.isDark ? Qt.rgba(1,1,1,0.05) : Qt.rgba(0.95,0.95,0.95,1) }
        } : null

        color: {
            if (!control.enabled) return control.isGhost ? "transparent" : (ThemeColors.isDark ? "#333333" : "#E5E5E5");
            
            if (control.isPrimary) {
                return control.pressed ? Qt.darker("#007AFF", 1.1) : "#007AFF"; // Apple Blue
            }
            if (control.isDanger) {
                return control.pressed ? Qt.darker("#FF3B30", 1.1) : "#FF3B30"; // Apple Red
            }
            if (control.isGhost) {
                return control.pressed ? Qt.rgba(0,0,0,0.05) : (control.hovered ? Qt.rgba(0,0,0,0.03) : "transparent");
            }
            
            // Secondary fallback (if gradient not active or flat)
            return ThemeColors.isDark ? "#4D4D4D" : "#FFFFFF";
        }

        border.width: (control.isPrimary || control.isDanger) ? 0 : 1
        border.color: {
             if (control.isGhost) return "transparent";
             return ThemeColors.isDark ? "#666666" : "#D1D1D1";
        }
        
        // macOS Focus Ring (Outer Glow)
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

    MouseArea {
        anchors.fill: parent
        cursorShape: control.enabled ? Qt.ArrowCursor : Qt.ArrowCursor // macOS uses Arrow, not Hand for buttons usually
        acceptedButtons: Qt.NoButton
    }
}
