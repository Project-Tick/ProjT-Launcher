// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme.js" as Theme

Dialog {
    id: offlineLoginDialog
    title: qsTr("Add Offline Account")
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel
    width: 400
    height: 250
    
    property string username: ""
    
    ColumnLayout {
        anchors.fill: parent
        spacing: Theme.spacingM
        
        // Info
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            radius: 8
            color: "#f59e0b15"
            
            RowLayout {
                anchors.fill: parent
                anchors.margins: Theme.spacingM
                spacing: Theme.spacingM
                
                Rectangle {
                    Layout.preferredWidth: 40
                    Layout.preferredHeight: 40
                    radius: 20
                    color: "#f59e0b"
                    
                    Label {
                        anchors.centerIn: parent
                        text: "!"
                        color: "white"
                        font.bold: true
                        font.pointSize: 16
                    }
                }
                
                Label {
                    Layout.fillWidth: true
                    text: qsTr("Offline accounts can only play single-player or on LAN servers that allow cracked accounts.")
                    color: Theme.textSecondary
                    wrapMode: Text.WordWrap
                    font.pointSize: Theme.fontSizeSmall
                }
            }
        }
        
        // Username field
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Account Details")
            
            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS
                
                RowLayout {
                    Layout.fillWidth: true
                    
                    Label {
                        text: qsTr("Username:")
                        Layout.preferredWidth: 80
                    }
                    
                    TextField {
                        id: usernameField
                        Layout.fillWidth: true
                        placeholderText: qsTr("Enter username...")
                        text: username
                        onTextChanged: username = text
                        
                        validator: RegularExpressionValidator {
                            regularExpression: /^[a-zA-Z0-9_]{3,16}$/
                        }
                    }
                }
                
                Label {
                    Layout.fillWidth: true
                    text: qsTr("Username must be 3-16 characters and contain only letters, numbers, and underscores.")
                    color: Theme.textSecondary
                    font.pointSize: Theme.fontSizeSmall - 1
                    wrapMode: Text.WordWrap
                }
            }
        }
        
        Item { Layout.fillHeight: true }
    }
    
    onAccepted: {
        if (ProjT && ProjT.accountsVM && usernameField.text.length >= 3) {
            ProjT.accountsVM.addOfflineAccount(usernameField.text)
        }
    }
}
