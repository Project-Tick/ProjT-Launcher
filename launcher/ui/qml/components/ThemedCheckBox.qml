// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2025 Project Tick
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import ProjTLauncher 1.0

CheckBox {
    id: control
    
    property string description: ""
    property bool useSwitch: false
    
    topPadding: 8
    bottomPadding: 8
    
    indicator: Rectangle {
        implicitWidth: control.useSwitch ? 44 : 20
        implicitHeight: control.useSwitch ? 24 : 20
        x: control.leftPadding
        y: parent.height / 2 - height / 2
        radius: control.useSwitch ? height / 2 : 4
        color: {
            if (control.useSwitch) {
                return control.checked ? ThemeColors.accent : ThemeColors.bg1;
            }
            return control.checked ? ThemeColors.accent : "transparent";
        }
        border.color: control.checked ? ThemeColors.accent : ThemeColors.border
        border.width: control.useSwitch ? 0 : 2
        
        Behavior on color { ColorAnimation { duration: 150 } }
        Behavior on border.color { ColorAnimation { duration: 150 } }

        // Checkmark for checkbox mode
        Text {
            visible: !control.useSwitch && control.checked
            anchors.centerIn: parent
            text: "✓"
            color: "#FFFFFF"
            font.pixelSize: 14
            font.bold: true
        }
        
        // Circle for switch mode
        Rectangle {
            visible: control.useSwitch
            x: control.checked ? parent.width - width - 3 : 3
            anchors.verticalCenter: parent.verticalCenter
            width: 18
            height: 18
            radius: 9
            color: "#FFFFFF"
            
            Behavior on x { NumberAnimation { duration: 150; easing.type: Easing.OutCubic } }
            
            // Subtle shadow
            layer.enabled: true
            layer.effect: Item {
                Rectangle {
                    anchors.fill: parent
                    anchors.margins: -1
                    radius: 10
                    color: Qt.rgba(0, 0, 0, 0.2)
                    z: -1
                }
            }
        }
    }

    contentItem: ColumnLayout {
        spacing: 2
        anchors.left: control.indicator.right
        anchors.leftMargin: 12
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        
        Text {
            text: control.text
            font.pixelSize: 13
            color: control.enabled ? ThemeColors.text : ThemeColors.textDisabled
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
        }
        
        Text {
            visible: control.description !== ""
            text: control.description
            font.pixelSize: 11
            color: ThemeColors.textSecondary
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
        }
    }
    
    background: Rectangle {
        color: control.hovered ? Qt.rgba(255, 255, 255, 0.03) : "transparent"
        radius: 6
    }
}
