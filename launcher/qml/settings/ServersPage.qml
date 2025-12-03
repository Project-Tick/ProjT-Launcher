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
 * Servers Page – Phase 11.C.2
 * Lists and manages multiplayer servers for instance
 */

Rectangle {
    id: root
    objectName: "serversPage"
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
                text: qsTr("Servers")
                font.pixelSize: 24
                font.weight: Font.Bold
                color: Theme.foreground
            }
            
            Item { Layout.fillWidth: true }
            
            Button {
                text: qsTr("Open servers.dat")
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
                onClicked: addServerDialog.open()
                
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
                text: qsTr("Edit")
                enabled: serversList.currentIndex >= 0
                onClicked: {
                    if (serversList.currentIndex >= 0) {
                        editServerDialog.serverIndex = serversList.currentIndex
                        editServerDialog.open()
                    }
                }
                
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
                enabled: serversList.currentIndex >= 0
                onClicked: deleteServerDialog.open()
                
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
            
            Rectangle {
                width: 1
                height: 24
                color: Theme.surface1
            }
            
            Button {
                text: qsTr("Move Up")
                enabled: serversList.currentIndex > 0
                onClicked: {
                    if (root.vm && serversList.currentIndex > 0) {
                        root.vm.moveServerUp(serversList.currentIndex)
                    }
                }
                
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
                text: qsTr("Move Down")
                enabled: serversList.currentIndex >= 0 && serversList.currentIndex < serversList.count - 1
                onClicked: {
                    if (root.vm && serversList.currentIndex >= 0) {
                        root.vm.moveServerDown(serversList.currentIndex)
                    }
                }
                
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
            
            Item { Layout.fillWidth: true }
            
            Button {
                text: qsTr("Refresh")
                onClicked: {
                    if (root.vm) root.vm.refreshServers()
                }
                
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
        
        // Servers List
        ListView {
            id: serversList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: Theme.spacingXS
            
            // Placeholder model - will be replaced with actual servers model
            model: ListModel {
                id: serversModel
            }
            
            // Empty state
            Text {
                anchors.centerIn: parent
                text: qsTr("No servers configured.\nAdd servers in-game or use the Add button.")
                horizontalAlignment: Text.AlignHCenter
                color: Theme.mutedForeground
                font.pixelSize: 14
                visible: serversList.count === 0
            }
            
            delegate: Rectangle {
                width: serversList.width
                height: 70
                color: serversList.currentIndex === index ? Theme.selection : Theme.surface0
                border.color: serversList.currentIndex === index ? Theme.accent : Theme.surface1
                border.width: 1
                radius: Theme.radiusS
                
                MouseArea {
                    anchors.fill: parent
                    onClicked: serversList.currentIndex = index
                    onDoubleClicked: {
                        // Edit server
                        editServerDialog.serverIndex = index
                        editServerDialog.open()
                    }
                }
                
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: Theme.spacingS
                    spacing: Theme.spacingM
                    
                    // Server icon placeholder
                    Rectangle {
                        width: 48
                        height: 48
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
                            text: "🖥️"
                            font.pixelSize: 24
                            visible: !model.icon
                        }
                    }
                    
                    ColumnLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        spacing: 2
                        
                        Text {
                            text: model.name || qsTr("Server %1").arg(index + 1)
                            color: Theme.foreground
                            font.weight: Font.DemiBold
                            font.pixelSize: 14
                        }
                        
                        Text {
                            text: model.address || "localhost:25565"
                            color: Theme.mutedForeground
                            font.pixelSize: 12
                            font.family: "monospace"
                        }
                        
                        Text {
                            text: model.motd || ""
                            color: Theme.mutedForeground
                            font.pixelSize: 11
                            elide: Text.ElideRight
                            Layout.fillWidth: true
                            visible: model.motd !== undefined
                        }
                    }
                    
                    // Status indicator
                    Rectangle {
                        width: 12
                        height: 12
                        radius: 6
                        color: {
                            if (model.status === "online") return Theme.green;
                            if (model.status === "offline") return Theme.red;
                            return Theme.yellow;
                        }
                        
                        ToolTip.visible: statusMouseArea.containsMouse
                        ToolTip.text: {
                            if (model.status === "online") return qsTr("Online - %1 players").arg(model.players || 0);
                            if (model.status === "offline") return qsTr("Offline");
                            return qsTr("Unknown");
                        }
                        
                        MouseArea {
                            id: statusMouseArea
                            anchors.fill: parent
                            hoverEnabled: true
                        }
                    }
                }
            }
        }
    }
    
    // Add Server Dialog
    Dialog {
        id: addServerDialog
        title: qsTr("Add Server")
        modal: true
        standardButtons: Dialog.Ok | Dialog.Cancel
        x: (root.width - width) / 2
        y: (root.height - height) / 2
        width: 400
        
        ColumnLayout {
            anchors.fill: parent
            spacing: Theme.spacingS
            
            Label {
                text: qsTr("Server Name:")
                color: Theme.foreground
            }
            
            TextField {
                id: newServerNameField
                Layout.fillWidth: true
                placeholderText: qsTr("My Server")
            }
            
            Label {
                text: qsTr("Server Address:")
                color: Theme.foreground
            }
            
            TextField {
                id: newServerAddressField
                Layout.fillWidth: true
                placeholderText: qsTr("server.example.com:25565")
            }
        }
        
        onAccepted: {
            if (root.vm && newServerNameField.text.length > 0 && newServerAddressField.text.length > 0) {
                root.vm.addServer(newServerNameField.text, newServerAddressField.text)
                newServerNameField.text = ""
                newServerAddressField.text = ""
            }
        }
    }
    
    // Edit Server Dialog
    Dialog {
        id: editServerDialog
        title: qsTr("Edit Server")
        modal: true
        standardButtons: Dialog.Ok | Dialog.Cancel
        x: (root.width - width) / 2
        y: (root.height - height) / 2
        width: 400
        
        property int serverIndex: -1
        
        ColumnLayout {
            anchors.fill: parent
            spacing: Theme.spacingS
            
            Label {
                text: qsTr("Server Name:")
                color: Theme.foreground
            }
            
            TextField {
                id: editServerNameField
                Layout.fillWidth: true
            }
            
            Label {
                text: qsTr("Server Address:")
                color: Theme.foreground
            }
            
            TextField {
                id: editServerAddressField
                Layout.fillWidth: true
            }
        }
        
        onOpened: {
            if (serverIndex >= 0 && root.vm) {
                // Load current server data
                editServerNameField.text = serversModel.get(serverIndex).name || ""
                editServerAddressField.text = serversModel.get(serverIndex).address || ""
            }
        }
        
        onAccepted: {
            if (root.vm && serverIndex >= 0) {
                root.vm.editServer(serverIndex, editServerNameField.text, editServerAddressField.text)
            }
        }
    }
    
    // Delete Server Dialog
    Dialog {
        id: deleteServerDialog
        title: qsTr("Delete Server")
        modal: true
        standardButtons: Dialog.Yes | Dialog.No
        x: (root.width - width) / 2
        y: (root.height - height) / 2
        
        Label {
            text: qsTr("Are you sure you want to delete this server?")
            color: Theme.red
            wrapMode: Text.WordWrap
        }
        
        onAccepted: {
            if (root.vm && serversList.currentIndex >= 0) {
                root.vm.deleteServer(serversList.currentIndex)
            }
        }
    }
}
