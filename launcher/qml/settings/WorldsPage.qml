// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  This file is part of ProjT Launcher and is licensed under
 *  the GNU General Public License version 3 or later.
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme.js" as Theme


/**
 * Worlds Page – Phase 11.C.2
 * Manages game worlds/saves for instance
 */

Rectangle {
    id: root
    objectName: "worldsPage"
    color: Theme.background
    
    property var vm: ProjT.instanceVM
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingM
        spacing: Theme.spacingM
        
        // Header
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingM
            
            Text {
                text: qsTr("Worlds")
                font.pixelSize: 24
                font.weight: Font.Bold
                color: Theme.foreground
            }
            
            Item { Layout.fillWidth: true }
            
            Button {
                text: qsTr("Open Folder")
                onClicked: if (root.vm) root.vm.openGameFolder()
                
                background: Rectangle {
                    color: parent.hovered ? Theme.surface1 : Theme.surface0
                    border.color: Theme.surface2
                    border.width: 1
                    radius: Theme.radiusS
                }
                
                contentItem: Text {
                    text: parent.text
                    color: Theme.foreground
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
        
        Text {
            text: root.vm && root.vm.instanceName ? root.vm.instanceName : qsTr("No instance selected")
            font.pixelSize: 14
            color: Theme.mutedForeground
            visible: root.vm !== null
        }
        
        // Toolbar
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingS
            
            Button {
                text: qsTr("Add")
                
                background: Rectangle {
                    color: parent.hovered ? Theme.surface1 : Theme.surface0
                    border.color: Theme.surface2
                    border.width: 1
                    radius: Theme.radiusS
                }
                
                contentItem: Text {
                    text: parent.text
                    color: Theme.foreground
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
            
            Button {
                text: qsTr("Copy")
                enabled: worldsList.currentIndex >= 0
                
                background: Rectangle {
                    color: parent.enabled ? (parent.hovered ? Theme.surface1 : Theme.surface0) : Theme.mantle
                    border.color: Theme.surface2
                    border.width: 1
                    radius: Theme.radiusS
                }
                
                contentItem: Text {
                    text: parent.text
                    color: parent.enabled ? Theme.foreground : Theme.mutedForeground
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
            
            Button {
                text: qsTr("Backup")
                enabled: worldsList.currentIndex >= 0
                
                background: Rectangle {
                    color: parent.enabled ? (parent.hovered ? Theme.surface1 : Theme.surface0) : Theme.mantle
                    border.color: Theme.surface2
                    border.width: 1
                    radius: Theme.radiusS
                }
                
                contentItem: Text {
                    text: parent.text
                    color: parent.enabled ? Theme.foreground : Theme.mutedForeground
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
            
            Button {
                text: qsTr("Delete")
                enabled: worldsList.currentIndex >= 0
                
                background: Rectangle {
                    color: parent.enabled ? (parent.hovered ? Theme.red : Theme.surface0) : Theme.mantle
                    border.color: parent.enabled ? Theme.red : Theme.surface2
                    border.width: 1
                    radius: Theme.radiusS
                }
                
                contentItem: Text {
                    text: parent.text
                    color: parent.enabled ? (parent.hovered ? Theme.base : Theme.red) : Theme.mutedForeground
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
            
            Item { Layout.fillWidth: true }
            
            Button {
                text: qsTr("Refresh")
                
                background: Rectangle {
                    color: parent.hovered ? Theme.surface1 : Theme.surface0
                    border.color: Theme.surface2
                    border.width: 1
                    radius: Theme.radiusS
                }
                
                contentItem: Text {
                    text: parent.text
                    color: Theme.foreground
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
        
        // Worlds List
        ListView {
            id: worldsList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: Theme.spacingXS
            
            // Placeholder model - will be replaced with actual worlds model
            model: ListModel {
                id: worldsModel
            }
            
            // Empty state
            Text {
                anchors.centerIn: parent
                text: qsTr("No worlds found.\nCreate or import worlds to see them here.")
                horizontalAlignment: Text.AlignHCenter
                color: Theme.mutedForeground
                font.pixelSize: 14
                visible: worldsList.count === 0
            }
            
            delegate: Rectangle {
                width: worldsList.width
                height: 80
                color: worldsList.currentIndex === index ? Theme.selection : Theme.surface0
                border.color: worldsList.currentIndex === index ? Theme.accent : Theme.surface1
                border.width: 1
                radius: Theme.radiusS
                
                MouseArea {
                    anchors.fill: parent
                    onClicked: worldsList.currentIndex = index
                    onDoubleClicked: {
                        // Open world folder
                    }
                }
                
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: Theme.spacingS
                    spacing: Theme.spacingM
                    
                    // World icon placeholder
                    Rectangle {
                        width: 64
                        height: 64
                        color: Theme.mantle
                        radius: Theme.radiusS
                        
                        Image {
                            anchors.fill: parent
                            anchors.margins: 4
                            source: model.icon || ""
                            fillMode: Image.PreserveAspectFit
                            visible: source !== ""
                        }
                        
                        Text {
                            anchors.centerIn: parent
                            text: "🌍"
                            font.pixelSize: 32
                            visible: !model.icon
                        }
                    }
                    
                    ColumnLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        spacing: 2
                        
                        Text {
                            text: model.name || qsTr("World %1").arg(index + 1)
                            color: Theme.foreground
                            font.weight: Font.DemiBold
                            font.pixelSize: 14
                        }
                        
                        Text {
                            text: model.gameMode || qsTr("Survival")
                            color: Theme.accent
                            font.pixelSize: 12
                        }
                        
                        Text {
                            text: model.lastPlayed || qsTr("Never played")
                            color: Theme.mutedForeground
                            font.pixelSize: 11
                        }
                    }
                    
                    ColumnLayout {
                        spacing: 2
                        
                        Text {
                            text: model.size || ""
                            color: Theme.mutedForeground
                            font.pixelSize: 11
                            horizontalAlignment: Text.AlignRight
                        }
                        
                        Text {
                            text: model.seed || ""
                            color: Theme.mutedForeground
                            font.pixelSize: 10
                            horizontalAlignment: Text.AlignRight
                        }
                    }
                }
            }
        }
    }
}
