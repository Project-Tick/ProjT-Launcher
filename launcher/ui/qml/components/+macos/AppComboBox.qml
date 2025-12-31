// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team

import QtQuick 2.15
import QtQuick.Controls 2.15
import ProjTLauncher 1.0

// MACOS NATIVE STYLE OVERRIDE
ComboBox {
    id: control

    font.family: "SF Pro Text"
    font.pixelSize: 13
    
    delegate: ItemDelegate {
        width: control.width
        contentItem: Text {
            text: control.textRole ? (Array.isArray(control.model) ? modelData[control.textRole] : model[control.textRole]) : modelData
            color: control.highlightedIndex === index ? "#FFFFFF" : (ThemeColors.isDark ? "#DFDFDF" : "#000000")
            font: control.font
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
        }
        background: Rectangle {
            color: control.highlightedIndex === index ? "#007AFF" : "transparent"
        }
        highlighted: control.highlightedIndex === index
    }

    // Double Arrow Indicator (Up/Down)
    indicator: Canvas {
        id: canvas
        x: control.width - width - control.rightPadding
        y: control.topPadding + (control.availableHeight - height) / 2
        width: 8
        height: 10
        contextType: "2d"

        Connections {
            target: control
            function onPressedChanged() { canvas.requestPaint(); }
        }

        onPaint: {
            context.reset();
            // Up Arrow
            context.moveTo(0, 4);
            context.lineTo(4, 0);
            context.lineTo(8, 4);
            // Down Arrow
            context.moveTo(0, 6);
            context.lineTo(4, 10);
            context.lineTo(8, 6);
            
            context.lineWidth = 1.5;
            context.strokeStyle = "#555555"; // Dark Gray Arrows
            context.stroke();
        }
    }

    contentItem: Text {
        leftPadding: 8
        rightPadding: control.indicator.width + control.spacing

        text: control.displayText
        font: control.font
        color: control.enabled ? (ThemeColors.isDark ? "#DFDFDF" : "#000000") : (ThemeColors.isDark ? "#666666" : "#999999")
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        implicitWidth: 120
        implicitHeight: 22
        radius: 5

        gradient: Gradient {
            GradientStop { position: 0.0; color: ThemeColors.isDark ? Qt.rgba(1,1,1,0.15) : Qt.rgba(1,1,1,1) }
            GradientStop { position: 1.0; color: ThemeColors.isDark ? Qt.rgba(1,1,1,0.05) : Qt.rgba(0.95,0.95,0.95,1) }
        }
        
        border.color: ThemeColors.isDark ? "#666666" : "#D1D1D1"
        border.width: 1

        // Focus Halo
        Rectangle {
            anchors.fill: parent
            anchors.margins: -3
            radius: parent.radius + 3
            color: "transparent"
            border.width: 2
            border.color: "#80007AFF"
            visible: control.activeFocus
        }
    }

    popup: Popup {
        y: control.height
        width: control.width
        implicitHeight: contentItem.implicitHeight
        padding: 4

        contentItem: ListView {
            clip: true
            implicitHeight: contentHeight
            model: control.popup.visible ? control.delegateModel : null
            currentIndex: control.highlightedIndex
            ScrollIndicator.vertical: ScrollIndicator { }
        }

        background: Rectangle {
            border.color: ThemeColors.isDark ? "#555555" : "#D1D1D1"
            color: ThemeColors.isDark ? "#2C2C2C" : "#FFFFFF"
            radius: 5
            
            // Drop Shadow could be added
        }
    }
}
