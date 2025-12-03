// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme.js" as Theme

Dialog {
    id: blockedModsDialog
    title: qsTr("Blocked Mods")
    modal: true
    standardButtons: Dialog.Ok
    width: 550
    height: 450
    
    property var blockedMods: []
    property string instanceName: ""
    
    ColumnLayout {
        anchors.fill: parent
        spacing: Theme.spacingM
        
        // Warning header
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            radius: 8
            color: "#ff5722"
            opacity: 0.15
            
            RowLayout {
                anchors.fill: parent
                anchors.margins: Theme.spacingM
                spacing: Theme.spacingM
                
                Rectangle {
                    Layout.preferredWidth: 40
                    Layout.preferredHeight: 40
                    radius: 20
                    color: "#ff5722"
                    
                    Label {
                        anchors.centerIn: parent
                        text: "!"
                        font.bold: true
                        font.pointSize: 18
                        color: "white"
                    }
                }
                
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 2
                    
                    Label {
                        text: qsTr("Some mods require manual download")
                        color: "#ff5722"
                        font.bold: true
                    }
                    
                    Label {
                        text: qsTr("These mods cannot be downloaded automatically due to licensing restrictions.")
                        color: Theme.textSecondary
                        font.pointSize: Theme.fontSizeSmall
                        wrapMode: Text.WordWrap
                    }
                }
            }
        }
        
        // Instructions
        Label {
            Layout.fillWidth: true
            text: qsTr("Please download the following mods manually and place them in the mods folder:")
            color: Theme.textPrimary
            wrapMode: Text.WordWrap
        }
        
        // Blocked mods list
        Frame {
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            ListView {
                id: modsList
                anchors.fill: parent
                clip: true
                model: blockedMods
                spacing: 2
                
                delegate: Rectangle {
                    width: modsList.width
                    height: 56
                    radius: 4
                    color: index % 2 === 0 ? "transparent" : Theme.backgroundAlt
                    
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: Theme.spacingS
                        spacing: Theme.spacingM
                        
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2
                            
                            Label {
                                text: modelData.name || qsTr("Unknown Mod")
                                color: Theme.textPrimary
                                font.bold: true
                            }
                            
                            Label {
                                text: modelData.version || ""
                                color: Theme.textSecondary
                                font.pointSize: Theme.fontSizeSmall
                                visible: text.length > 0
                            }
                        }
                        
                        Button {
                            text: qsTr("Open Page")
                            icon.name: "internet-web-browser"
                            visible: modelData.url && modelData.url.length > 0
                            onClicked: {
                                Qt.openUrlExternally(modelData.url)
                            }
                        }
                        
                        Button {
                            text: qsTr("Copy URL")
                            icon.name: "edit-copy"
                            visible: modelData.url && modelData.url.length > 0
                            onClicked: {
                                if (ProjT) {
                                    ProjT.copyToClipboard(modelData.url)
                                }
                            }
                        }
                    }
                }
                
                ScrollBar.vertical: ScrollBar {}
            }
            
            Label {
                anchors.centerIn: parent
                text: qsTr("No blocked mods")
                color: Theme.textSecondary
                visible: blockedMods.length === 0
            }
        }
        
        // Actions
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingM
            
            Button {
                text: qsTr("Open Mods Folder")
                icon.name: "folder-open"
                onClicked: {
                    if (ProjT && ProjT.instanceVM) {
                        ProjT.instanceVM.openModsFolder()
                    }
                }
            }
            
            Item { Layout.fillWidth: true }
            
            Label {
                text: qsTr("%1 mod(s) require manual download").arg(blockedMods.length)
                color: Theme.textSecondary
                font.pointSize: Theme.fontSizeSmall
            }
        }
    }
}
