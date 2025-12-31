// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team

import QtQuick 2.15
import QtQuick.Controls 2.15
import ProjTLauncher 1.0

ComboBox {
    id: control

    // Colors
    property color textColor: ThemeColors.text
    property color backgroundColor: ThemeColors.surface3
    
    font: ThemeColors.fontBody

    delegate: ItemDelegate {
        width: control.width
        contentItem: Text {
            text: control.textRole ? (Array.isArray(control.model) ? modelData[control.textRole] : model[control.textRole]) : modelData
            color: control.highlightedIndex === index ? ThemeColors.accentText : ThemeColors.text
            font: control.font
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
        }
        background: Rectangle {
            color: control.highlightedIndex === index ? ThemeColors.accent : "transparent"
        }
        highlighted: control.highlightedIndex === index
    }

    indicator: Canvas {
        id: canvas
        x: control.width - width - control.rightPadding
        y: control.topPadding + (control.availableHeight - height) / 2
        width: 12
        height: 8
        contextType: "2d"

        Connections {
            target: control
            function onPressedChanged() { canvas.requestPaint(); }
        }

        onPaint: {
            context.reset();
            context.moveTo(0, 0);
            context.lineTo(width, 0);
            context.lineTo(width / 2, height);
            context.closePath();
            context.fillStyle = control.pressed ? ThemeColors.text : ThemeColors.textSecondary;
            context.fill();
        }
    }

    contentItem: Text {
        leftPadding: 0
        rightPadding: control.indicator.width + control.spacing

        text: control.displayText
        font: control.font
        color: control.enabled ? ThemeColors.text : ThemeColors.textDisabled
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        implicitWidth: 120
        implicitHeight: 36
        color: control.enabled ? backgroundColor : ThemeColors.bg1
        radius: ThemeColors.radiusM
        
        border.color: control.activeFocus ? ThemeColors.borderFocus : (control.hovered ? ThemeColors.textMuted : "transparent")
        border.width: control.activeFocus ? 2 : (control.hovered ? 1 : 0)
    }

    popup: Popup {
        y: control.height - 1
        width: control.width
        implicitHeight: contentItem.implicitHeight
        padding: 1

        contentItem: ListView {
            clip: true
            implicitHeight: contentHeight
            model: control.popup.visible ? control.delegateModel : null
            currentIndex: control.highlightedIndex
            ScrollIndicator.vertical: ScrollIndicator { }
        }

        background: Rectangle {
            border.color: ThemeColors.border
            color: ThemeColors.surface
            radius: ThemeColors.radiusM
        }
    }
}
