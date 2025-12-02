// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  Accounts settings page
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme.js" as Theme

Rectangle {
    id: accountsPage
    color: Theme.background
    
    property var vm: ProjT.launcherVM
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingM
        spacing: Theme.spacingM
        
        // Header
        RowLayout {
            Layout.fillWidth: true
            
            Label {
                text: qsTr("Accounts")
                font.pointSize: 12
                font.bold: true
                color: Theme.textPrimary
            }
            
            Item { Layout.fillWidth: true }
            
            Button {
                text: qsTr("Add Microsoft")
                icon.name: "list-add"
                onClicked: {
                    if (vm) {
                        vm.openAccountsManager()
                    }
                }
            }
            
            Button {
                text: qsTr("Add Offline")
                icon.name: "list-add"
                enabled: false
                ToolTip.text: qsTr("Offline accounts require configuration")
                ToolTip.visible: hovered
            }
        }
        
        // Account list
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: Theme.surfaceBackground
            border.color: Theme.border
            border.width: 1
            radius: Theme.radiusS
            
            ListView {
                id: accountList
                anchors.fill: parent
                anchors.margins: 1
                clip: true
                
                model: [] // Would come from AccountListModel
                
                delegate: ItemDelegate {
                    width: accountList.width
                    height: 60
                    
                    contentItem: RowLayout {
                        spacing: Theme.spacingM
                        
                        // Avatar
                        Rectangle {
                            Layout.preferredWidth: 40
                            Layout.preferredHeight: 40
                            radius: 4
                            color: Theme.border
                            
                            Image {
                                anchors.centerIn: parent
                                width: 32
                                height: 32
                                source: modelData.avatarUrl || ""
                                fillMode: Image.PreserveAspectFit
                            }
                        }
                        
                        // Name and type
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2
                            
                            Label {
                                text: modelData.username || "Unknown"
                                color: Theme.textPrimary
                                font.bold: true
                            }
                            
                            Label {
                                text: modelData.type || "Microsoft"
                                color: Theme.textSecondary
                                font.pointSize: 9
                            }
                        }
                        
                        // Status
                        Label {
                            text: modelData.active ? qsTr("Active") : ""
                            color: Theme.success
                            font.pointSize: 9
                        }
                        
                        // Actions
                        Button {
                            text: qsTr("Set Active")
                            visible: !modelData.active
                            onClicked: {
                                // Set as active account
                            }
                        }
                        
                        Button {
                            text: qsTr("Remove")
                            onClicked: {
                                // Remove account
                            }
                        }
                    }
                }
                
                ScrollBar.vertical: ScrollBar {}
            }
            
            // Empty state
            ColumnLayout {
                anchors.centerIn: parent
                visible: accountList.count === 0
                spacing: Theme.spacingM
                
                Label {
                    Layout.alignment: Qt.AlignHCenter
                    text: "👤"
                    font.pointSize: 48
                    opacity: 0.5
                }
                
                Label {
                    Layout.alignment: Qt.AlignHCenter
                    text: qsTr("No accounts added")
                    color: Theme.textSecondary
                    font.pointSize: 12
                }
                
                Label {
                    Layout.alignment: Qt.AlignHCenter
                    text: qsTr("Add a Microsoft account to play Minecraft")
                    color: Theme.textSecondary
                }
                
                Button {
                    Layout.alignment: Qt.AlignHCenter
                    text: qsTr("Add Microsoft Account")
                    onClicked: {
                        if (vm) {
                            vm.openAccountsManager()
                        }
                    }
                }
            }
        }
        
        // Info
        Label {
            text: qsTr("Note: Account data is stored securely using system keychain.")
            color: Theme.textSecondary
            font.pointSize: 9
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }
    }
}
