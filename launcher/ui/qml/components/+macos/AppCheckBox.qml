// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team

import QtQuick 2.15
import QtQuick.Controls 2.15
import ProjTLauncher 1.0

// MACOS NATIVE STYLE OVERRIDE
CheckBox {
    id: control

    font.family: "SF Pro Text"
    font.pixelSize: 13
    spacing: 6

    indicator: Rectangle {
        implicitWidth: 14
        implicitHeight: 14
        x: control.leftPadding
        y: parent.height / 2 - height / 2
        radius: 3
        
        color: control.checked ? "#007AFF" : (ThemeColors.isDark ? "#333333" : "#FFFFFF")
        border.color: control.checked ? "#0064C6" : (ThemeColors.isDark ? "#666666" : "#CCCCCC")
        border.width: 1

        // Checkmark (White)
        Canvas {
            anchors.fill: parent
            anchors.margins: 1 // Tights margins
            visible: control.checked
            contextType: "2d"
            onPaint: {
                context.reset();
                context.strokeStyle = "#FFFFFF";
                context.lineWidth = 2;
                context.lineCap = "round"
                context.lineJoin = "round"
                
                // Draw Check
                context.moveTo(2, 6);
                context.lineTo(5, 9);
                context.lineTo(10, 3);
                context.stroke();
            }
        }
    }

    contentItem: Text {
        text: control.text
        font: control.font
        opacity: enabled ? 1.0 : 0.5
        color: ThemeColors.isDark ? "#DFDFDF" : "#000000"
        verticalAlignment: Text.AlignVCenter
        leftPadding: control.indicator.width + control.spacing
    }
}
