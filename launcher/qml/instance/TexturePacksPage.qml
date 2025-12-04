// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme.js" as Theme

Rectangle {
    id: texturePacksPage
    color: Theme.background
    
    property var vm: ProjT.instanceVM
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingM
        spacing: Theme.spacingS
        
        // Header
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingS
            
            Label {
                text: qsTr("Texture Packs (Legacy)")
                font.pointSize: 14
                font.bold: true
                color: Theme.textPrimary
            }
            
            Item { Layout.fillWidth: true }
            
            Button {
                text: qsTr("Add")
                icon.name: "list-add"
                onClicked: {
                    if (vm) vm.browseForTexturePacks()
                }
            }
            
            Button {
                text: qsTr("Refresh")
                icon.name: "view-refresh"
                onClicked: {
                    if (vm) vm.refreshTexturePacks()
                }
            }
        }
        
        Label {
            text: qsTr("Texture packs are used in Minecraft versions before 1.6. For newer versions, use Resource Packs.")
            color: Theme.textSecondary
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }
        
        // Texture pack list
        Frame {
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            ListView {
                id: texturesList
                anchors.fill: parent
                clip: true
                model: vm ? vm.texturePacksModel : []
                
                delegate: ItemDelegate {
                    width: texturesList.width
                    height: 56
                    
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: Theme.spacingS
                        spacing: Theme.spacingS
                        
                        Rectangle {
                            Layout.preferredWidth: 40
                            Layout.preferredHeight: 40
                            color: Theme.surfaceVariant
                            radius: 4
                            
                            Label {
                                anchors.centerIn: parent
                                text: "🖼️"
                                font.pointSize: 16
                            }
                        }
                        
                        Label {
                            text: model.name || model.fileName || ""
                            color: Theme.textPrimary
                            font.bold: true
                            elide: Text.ElideRight
                            Layout.fillWidth: true
                        }
                        
                        ToolButton {
                            icon.name: "edit-delete"
                            onClicked: {
                                if (vm) vm.removeTexturePack(index)
                            }
                        }
                    }
                }
                
                ScrollBar.vertical: ScrollBar {}
            }
            
            Label {
                anchors.centerIn: parent
                visible: texturesList.count === 0
                text: qsTr("No texture packs installed.")
                color: Theme.textSecondary
            }
        }
        
        Label {
            text: vm ? qsTr("%1 texture packs").arg(vm.texturePacksCount || 0) : ""
            color: Theme.textSecondary
        }
    }
}
