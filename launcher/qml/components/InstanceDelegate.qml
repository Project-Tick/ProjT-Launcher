// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  This file is part of ProjT Launcher and is licensed under
 *  the GNU General Public License version 3 or later.
 *
 *  If this file includes work from previous open-source projects,
 *  their original copyright and license notices are preserved below.
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme.js" as Theme

/**
 * InstanceDelegate – Individual Instance Card/List Item
 * 
 * Renders a single instance in the instance list.
 * Features:
 * - Instance icon display
 * - Name and group info
 * - Status indicators (running, last played)
 * - Context menu (right-click)
 * - Double-click to launch
 * - Drag-drop support
 * 
 * Properties:
 * - instanceId: string
 * - instanceName: string
 * - instanceGroup: string
 * - iconPath: string
 * - isSelected: bool
 * - isRunning: bool
 * 
 * Signals:
 * - clicked(instanceId)
 * - doubleClicked(instanceId)
 * - rightClicked(instanceId, mouseX, mouseY)
 */

Rectangle {
    id: delegate
    
    // Properties
    property string instanceId: ""
    property string instanceName: ""
    property string instanceGroup: ""
    property string iconPath: ""
    property bool isSelected: false
    property bool isRunning: false
    property string lastPlayedText: ""
    
    // Signals
    signal clicked(string instanceId)
    signal doubleClicked(string instanceId)
    signal rightClicked(string instanceId, int mouseX, int mouseY)
    
    Component.onCompleted: {
        console.log("[InstanceDelegate] Created - name:", instanceName, "id:", instanceId)
    }
    
    height: 56
    color: isSelected ? "#2a3340" : (mouseArea.containsMouse ? "#1f2228" : Theme.surface)
    border.color: isSelected ? Theme.accent : "#323742"
    border.width: isSelected ? 1 : 0
    radius: Theme.radius
    
    Behavior on color {
        ColorAnimation { duration: 100 }
    }
    
    RowLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingS
        spacing: Theme.spacingS
        
        // === Instance Icon ===
        Rectangle {
            width: 40
            height: 40
            radius: Theme.radius
            color: "#3d4d60"
            Layout.alignment: Qt.AlignVCenter
            
            Image {
                anchors.fill: parent
                anchors.margins: 2
                source: iconPath ? ("file://" + iconPath) : ""
                sourceSize: Qt.size(36, 36)
                fillMode: Image.PreserveAspectFit
                smooth: true
                
                // Fallback text if no icon
                Rectangle {
                    anchors.fill: parent
                    visible: !parent.status || parent.status === Image.Error
                    color: "#3d4d60"
                    
                    Text {
                        anchors.centerIn: parent
                        text: instanceName.charAt(0).toUpperCase()
                        font.pointSize: 16
                        font.bold: true
                        color: Theme.textPrimary
                    }
                }
            }
        }
        
        // === Instance Info ===
        ColumnLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingXS
            
            RowLayout {
                Layout.fillWidth: true
                spacing: Theme.spacingS
                
                Text {
                    text: instanceName
                    color: Theme.textPrimary
                    font.pointSize: 12
                    font.bold: true
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }
                
                // Running indicator
                Rectangle {
                    width: 24
                    height: 24
                    radius: 12
                    visible: isRunning
                    color: "#2d7a2d"
                    
                    Text {
                        anchors.centerIn: parent
                        text: qsTr("R")
                        color: "#7cff7c"
                        font.pointSize: 10
                        font.bold: true
                    }
                }
            }
            
            RowLayout {
                Layout.fillWidth: true
                spacing: Theme.spacingS
                
                Text {
                    text: instanceGroup ? (instanceGroup) : qsTr("No Group")
                    color: Theme.textSecondary
                    font.pointSize: 10
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }
                
                // Last played info
                Text {
                    text: lastPlayedText
                    color: Theme.textSecondary
                    font.pointSize: 9
                    font.italic: true
                    Layout.alignment: Qt.AlignRight
                }
            }
        }
        
        // === Launch Button ===
        Button {
            id: playButton
            text: isRunning ? qsTr("Running...") : qsTr("Play")
            implicitHeight: 36
            implicitWidth: 56
            enabled: !isRunning
            Layout.alignment: Qt.AlignVCenter
            
            background: Rectangle {
                radius: Theme.radius
                color: playButton.enabled ? 
                       (playButton.hovered ? "#1e7e1e" : "#1a6b1a") :
                       "#555555"
                border.color: "#2d7a2d"
                border.width: 1
            }
            
            contentItem: Text {
                text: playButton.text
                color: playButton.enabled ? "#7cff7c" : "#999999"
                font.pointSize: 11
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            
            onClicked: delegate.doubleClicked(instanceId)
        }
    }
    
    // === Mouse Area for Selection & Context Menu ===
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        
        onClicked: function(mouse) {
            if (mouse.button === Qt.LeftButton) {
                delegate.clicked(instanceId)
            } else if (mouse.button === Qt.RightButton) {
                // Pass delegate-local mouse coordinates
                delegate.rightClicked(instanceId, mouse.x, mouse.y)
            }
        }
        
        onDoubleClicked: function(mouse) {
            if (mouse.button === Qt.LeftButton) {
                delegate.doubleClicked(instanceId)
            }
        }
    }
    
    // === Tooltip ===
    ToolTip.text: qsTr("Left-click to select, double-click to launch, right-click for more options")
    ToolTip.visible: mouseArea.containsMouse && !isRunning
    ToolTip.delay: 800
}
