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
 * Managed Pack Page – Phase 11.C.3
 * Displays information about managed pack (CurseForge, Modrinth, etc.)
 */

Rectangle {
    id: root
    objectName: "managedPackPage"
    color: Theme.background
    
    property var vm: ProjT.instanceVM
    property bool isManagedPack: vm ? vm.isManagedPack : false
    
    Flickable {
        anchors.fill: parent
        anchors.margins: Theme.spacingM
        contentHeight: column.height
        clip: true
        
        ColumnLayout {
            id: column
            width: parent.width
            spacing: Theme.spacingM
            
            Text {
                text: qsTr("Managed Pack")
                font.pixelSize: 24
                font.weight: Font.Bold
                color: Theme.foreground
            }
            
            Text {
                text: root.vm && root.vm.instanceName ? root.vm.instanceName : qsTr("No instance selected")
                font.pixelSize: 14
                color: Theme.mutedForeground
                visible: root.vm !== null
            }
            
            // Not a managed pack notice
            Rectangle {
                Layout.fillWidth: true
                height: 100
                color: Theme.mantle
                border.color: Theme.surface1
                border.width: 1
                radius: Theme.radiusS
                visible: !root.isManagedPack
                
                ColumnLayout {
                    anchors.centerIn: parent
                    spacing: Theme.spacingS
                    
                    Text {
                        text: "📦"
                        font.pixelSize: 32
                        horizontalAlignment: Text.AlignHCenter
                        Layout.alignment: Qt.AlignHCenter
                    }
                    
                    Text {
                        text: qsTr("This instance is not a managed pack")
                        color: Theme.mutedForeground
                        font.pixelSize: 14
                        horizontalAlignment: Text.AlignHCenter
                        Layout.alignment: Qt.AlignHCenter
                    }
                }
            }
            
            // Pack Information
            GroupBox {
                title: qsTr("Pack Information")
                Layout.fillWidth: true
                visible: root.isManagedPack
                
                background: Rectangle {
                    color: Theme.mantle
                    border.color: Theme.surface1
                    border.width: 1
                    radius: Theme.radiusS
                }
                
                label: Text {
                    text: parent.title
                    color: Theme.foreground
                    font.weight: Font.DemiBold
                    leftPadding: Theme.spacingS
                }
                
                GridLayout {
                    width: parent.width
                    columns: 2
                    columnSpacing: Theme.spacingL
                    rowSpacing: Theme.spacingS
                    
                    Text { 
                        text: qsTr("Platform:")
                        color: Theme.mutedForeground
                        font.pixelSize: 13
                    }
                    RowLayout {
                        spacing: Theme.spacingS
                        
                        Image {
                            width: 20
                            height: 20
                            source: Theme.platformIcon(root.vm ? root.vm.managedPackType : "")
                            visible: root.vm && root.vm.managedPackType
                        }
                        
                        Text { 
                            text: root.vm ? root.vm.managedPackType : ""
                            color: Theme.foreground
                            font.pixelSize: 13
                        }
                    }
                    
                    Text { 
                        text: qsTr("Pack Name:")
                        color: Theme.mutedForeground
                        font.pixelSize: 13
                    }
                    Text { 
                        text: root.vm ? root.vm.managedPackName : ""
                        color: Theme.foreground
                        font.pixelSize: 13
                        font.weight: Font.DemiBold
                    }
                    
                    Text { 
                        text: qsTr("Version:")
                        color: Theme.mutedForeground
                        font.pixelSize: 13
                    }
                    Text { 
                        text: root.vm ? root.vm.managedPackVersionName : ""
                        color: Theme.accent
                        font.pixelSize: 13
                    }
                }
            }
            
            // Update Section
            GroupBox {
                title: qsTr("Updates")
                Layout.fillWidth: true
                visible: root.isManagedPack
                
                background: Rectangle {
                    color: Theme.mantle
                    border.color: Theme.surface1
                    border.width: 1
                    radius: Theme.radiusS
                }
                
                label: Text {
                    text: parent.title
                    color: Theme.foreground
                    font.weight: Font.DemiBold
                    leftPadding: Theme.spacingS
                }
                
                ColumnLayout {
                    width: parent.width
                    spacing: Theme.spacingM
                    
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: Theme.spacingM
                        
                        Rectangle {
                            width: 12
                            height: 12
                            radius: 6
                            color: root.vm && root.vm.hasUpdateAvailable ? Theme.green : Theme.mutedForeground
                        }
                        
                        Text {
                            text: root.vm && root.vm.hasUpdateAvailable 
                                ? qsTr("Update available!") 
                                : qsTr("No updates available")
                            color: root.vm && root.vm.hasUpdateAvailable ? Theme.green : Theme.foreground
                            font.pixelSize: 14
                        }
                        
                        Item { Layout.fillWidth: true }
                        
                        Button {
                            text: qsTr("Check for Updates")
                            
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
                    
                    Button {
                        text: qsTr("Update Pack")
                        visible: root.vm && root.vm.hasUpdateAvailable
                        
                        background: Rectangle {
                            color: parent.hovered ? Qt.lighter(Theme.accent, 1.1) : Theme.accent
                            radius: Theme.radiusS
                        }
                        
                        contentItem: Text {
                            text: parent.text
                            color: Theme.base
                            font.weight: Font.DemiBold
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                }
            }
            
            // Actions
            GroupBox {
                title: qsTr("Actions")
                Layout.fillWidth: true
                visible: root.isManagedPack
                
                background: Rectangle {
                    color: Theme.mantle
                    border.color: Theme.surface1
                    border.width: 1
                    radius: Theme.radiusS
                }
                
                label: Text {
                    text: parent.title
                    color: Theme.foreground
                    font.weight: Font.DemiBold
                    leftPadding: Theme.spacingS
                }
                
                RowLayout {
                    width: parent.width
                    spacing: Theme.spacingM
                    
                    Button {
                        text: qsTr("View Online")
                        
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
                        text: qsTr("Export Pack")
                        
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
                    
                    Item { Layout.fillWidth: true }
                }
            }
            
            Item { Layout.fillHeight: true }
        }
    }
}
