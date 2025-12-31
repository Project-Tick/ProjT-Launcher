// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team

import QtQuick 2.15
import QtQuick.Controls 2.15
import ProjTLauncher 1.0

CheckBox {
    id: control
    
    font: ThemeColors.fontBody

    indicator: Rectangle {
        implicitWidth: 20
        implicitHeight: 20
        x: control.leftPadding
        y: parent.height / 2 - height / 2
        radius: 3
        color: control.checked ? ThemeColors.accent : "transparent"
        border.color: control.checked ? ThemeColors.accent : ThemeColors.textSecondary
        border.width: 1

        // Checkmark
        Canvas {
            anchors.fill: parent
            anchors.margins: 4
            visible: control.checked
            onPaint: {
                var ctx = getContext("2d")
                ctx.reset()
                ctx.lineWidth = 2
                ctx.strokeStyle = ThemeColors.accentText
                ctx.moveTo(0, height * 0.5)
                ctx.lineTo(width * 0.4, height)
                ctx.lineTo(width, 0)
                ctx.stroke()
            }
        }
    }

    contentItem: Text {
        text: control.text
        font: control.font
        opacity: enabled ? 1.0 : 0.3
        color: ThemeColors.text
        verticalAlignment: Text.AlignVCenter
        leftPadding: control.indicator.width + control.spacing
    }
}
