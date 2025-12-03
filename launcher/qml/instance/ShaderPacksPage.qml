// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme.js" as Theme

Rectangle {
    id: shaderPacksPage
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
                text: qsTr("Shader Packs")
                font.pointSize: 14
                font.bold: true
                color: Theme.textPrimary
            }
            
            Item { Layout.fillWidth: true }
            
            Button {
                text: qsTr("Add")
                icon.name: "list-add"
                onClicked: {
                    if (vm) vm.browseForShaderPacks()
                }
            }
            
            Button {
                text: qsTr("Download")
                icon.name: "download"
                onClicked: {
                    if (vm) vm.openShaderPackDownload()
                }
            }
            
            Button {
                text: qsTr("Refresh")
                icon.name: "view-refresh"
                onClicked: {
                    if (vm) vm.refreshShaderPacks()
                }
            }
        }
        
        // Shader pack list
        Frame {
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            ListView {
                id: shadersList
                anchors.fill: parent
                clip: true
                model: vm ? vm.shaderPacksModel : []
                
                delegate: ItemDelegate {
                    width: shadersList.width
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
                                text: "✨"
                                font.pointSize: 16
                            }
                        }
                        
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2
                            
                            Label {
                                text: model.name || model.fileName || ""
                                color: Theme.textPrimary
                                font.bold: true
                                elide: Text.ElideRight
                                Layout.fillWidth: true
                            }
                            
                            Label {
                                text: model.fileSize || ""
                                color: Theme.textSecondary
                                font.pointSize: 9
                            }
                        }
                        
                        ToolButton {
                            icon.name: "edit-delete"
                            onClicked: {
                                if (vm) vm.deleteShaderPack(index)
                            }
                        }
                    }
                }
                
                ScrollBar.vertical: ScrollBar {}
            }
            
            Label {
                anchors.centerIn: parent
                visible: shadersList.count === 0
                text: qsTr("No shader packs installed.\nShader packs require OptiFine or Iris.")
                color: Theme.textSecondary
                horizontalAlignment: Text.AlignHCenter
            }
        }
        
        Label {
            text: vm ? qsTr("%1 shader packs").arg(vm.shaderPacksCount || 0) : ""
            color: Theme.textSecondary
        }
    }
}
