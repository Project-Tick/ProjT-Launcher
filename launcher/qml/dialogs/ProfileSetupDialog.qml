// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme.js" as Theme

Dialog {
    id: profileSetupDialog
    title: qsTr("Complete Profile Setup")
    modal: true
    width: 450
    height: 350
    standardButtons: Dialog.NoButton
    closePolicy: Dialog.NoAutoClose
    
    property var vm: ProjT ? ProjT.accountsVM : null
    property var account: null
    property string gameName: ""
    
    ColumnLayout {
        anchors.fill: parent
        spacing: Theme.spacingM
        
        // Welcome message
        ColumnLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
            spacing: Theme.spacingS
            
            Rectangle {
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: 80
                Layout.preferredHeight: 80
                radius: 40
                color: Theme.accent
                
                Image {
                    anchors.fill: parent
                    anchors.margins: 8
                    source: account ? account.avatarUrl : ""
                    fillMode: Image.PreserveAspectFit
                    visible: status === Image.Ready
                }
                
                Label {
                    anchors.centerIn: parent
                    text: account && account.username ? account.username.charAt(0).toUpperCase() : "?"
                    font.pointSize: 28
                    color: "white"
                    visible: parent.children[0].status !== Image.Ready
                }
            }
            
            Label {
                text: qsTr("Welcome, %1!").arg(account ? account.username : "")
                color: Theme.textPrimary
                font.bold: true
                font.pointSize: Theme.fontSizeMedium
                Layout.alignment: Qt.AlignHCenter
            }
        }
        
        // Game name setup
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("In-Game Name")
            
            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS
                
                Label {
                    Layout.fillWidth: true
                    text: qsTr("Your Microsoft account doesn't have a Minecraft profile yet. Please create one:")
                    color: Theme.textSecondary
                    wrapMode: Text.WordWrap
                }
                
                TextField {
                    id: gameNameField
                    Layout.fillWidth: true
                    placeholderText: qsTr("Enter your desired in-game name...")
                    text: gameName
                    onTextChanged: gameName = text
                    
                    validator: RegularExpressionValidator {
                        regularExpression: /^[a-zA-Z0-9_]{3,16}$/
                    }
                }
                
                Label {
                    Layout.fillWidth: true
                    text: qsTr("Must be 3-16 characters. Only letters, numbers, and underscores allowed.")
                    color: Theme.textSecondary
                    font.pointSize: Theme.fontSizeSmall - 1
                }
            }
        }
        
        // Status message
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: statusLabel.implicitHeight + Theme.spacingM * 2
            radius: 8
            color: vm && vm.profileSetupError ? "#ef444420" : "transparent"
            visible: vm && (vm.profileSetupError || vm.profileSetupBusy)
            
            RowLayout {
                anchors.fill: parent
                anchors.margins: Theme.spacingS
                
                BusyIndicator {
                    Layout.preferredWidth: 24
                    Layout.preferredHeight: 24
                    running: vm ? vm.profileSetupBusy : false
                    visible: running
                }
                
                Label {
                    id: statusLabel
                    Layout.fillWidth: true
                    text: vm ? (vm.profileSetupError || vm.profileSetupStatus) : ""
                    color: vm && vm.profileSetupError ? "#ef4444" : Theme.textSecondary
                    wrapMode: Text.WordWrap
                }
            }
        }
        
        Item { Layout.fillHeight: true }
        
        // Buttons
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingM
            
            Button {
                text: qsTr("Cancel")
                onClicked: profileSetupDialog.reject()
            }
            
            Item { Layout.fillWidth: true }
            
            Button {
                text: qsTr("Create Profile")
                highlighted: true
                enabled: gameNameField.text.length >= 3 && (!vm || !vm.profileSetupBusy)
                onClicked: {
                    if (vm && account) {
                        vm.createGameProfile(account.id, gameNameField.text)
                    }
                }
            }
        }
    }
    
    Connections {
        target: vm
        function onProfileSetupComplete() {
            profileSetupDialog.accept()
        }
    }
}
