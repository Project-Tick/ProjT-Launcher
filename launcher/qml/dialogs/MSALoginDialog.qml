// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme.js" as Theme

Dialog {
    id: msaLoginDialog
    title: qsTr("Add Microsoft Account")
    modal: true
    closePolicy: Popup.NoAutoClose
    width: 500
    height: 400
    standardButtons: Dialog.Cancel
    
    property var vm: ProjT.accountsVM
    property string loginUrl: ""
    property string userCode: ""
    property int stage: 0 // 0: waiting, 1: code ready, 2: authenticating, 3: done
    
    ColumnLayout {
        anchors.fill: parent
        spacing: Theme.spacingM
        
        Image {
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: 64
            Layout.preferredHeight: 64
            source: "qrc:/icons/multimc/scalable/accounts.svg"
            fillMode: Image.PreserveAspectFit
        }
        
        Label {
            text: qsTr("Sign in with Microsoft")
            font.bold: true
            font.pointSize: 14
            color: Theme.textPrimary
            Layout.alignment: Qt.AlignHCenter
        }
        
        // Stage 0: Waiting for code
        ColumnLayout {
            visible: stage === 0
            Layout.fillWidth: true
            spacing: Theme.spacingS
            
            BusyIndicator {
                Layout.alignment: Qt.AlignHCenter
                running: stage === 0
            }
            
            Label {
                text: qsTr("Initializing login...")
                color: Theme.textSecondary
                Layout.alignment: Qt.AlignHCenter
            }
        }
        
        // Stage 1: Code ready
        ColumnLayout {
            visible: stage === 1
            Layout.fillWidth: true
            spacing: Theme.spacingM
            
            Label {
                text: qsTr("1. Go to the following URL:")
                color: Theme.textPrimary
                Layout.fillWidth: true
            }
            
            RowLayout {
                Layout.fillWidth: true
                
                TextField {
                    id: urlField
                    Layout.fillWidth: true
                    text: loginUrl
                    readOnly: true
                    selectByMouse: true
                }
                
                Button {
                    text: qsTr("Open")
                    onClicked: Qt.openUrlExternally(loginUrl)
                }
                
                Button {
                    text: qsTr("Copy")
                    onClicked: {
                        urlField.selectAll()
                        urlField.copy()
                    }
                }
            }
            
            Label {
                text: qsTr("2. Enter this code:")
                color: Theme.textPrimary
                Layout.fillWidth: true
            }
            
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 60
                color: Theme.surfaceVariant
                radius: Theme.radius
                
                RowLayout {
                    anchors.centerIn: parent
                    spacing: Theme.spacingM
                    
                    Label {
                        text: userCode
                        font.family: "Noto Sans Mono"
                        font.pointSize: 24
                        font.bold: true
                        color: Theme.accent
                    }
                    
                    Button {
                        text: qsTr("Copy Code")
                        onClicked: {
                            if (vm) vm.copyCodeToClipboard(userCode)
                        }
                    }
                }
            }
            
            Label {
                text: qsTr("3. Sign in with your Microsoft account on the webpage")
                color: Theme.textPrimary
                Layout.fillWidth: true
            }
            
            Label {
                text: qsTr("Waiting for authentication...")
                color: Theme.textSecondary
                Layout.alignment: Qt.AlignHCenter
            }
            
            BusyIndicator {
                Layout.alignment: Qt.AlignHCenter
                running: stage === 1
            }
        }
        
        // Stage 2: Authenticating
        ColumnLayout {
            visible: stage === 2
            Layout.fillWidth: true
            spacing: Theme.spacingS
            
            BusyIndicator {
                Layout.alignment: Qt.AlignHCenter
                running: stage === 2
            }
            
            Label {
                text: qsTr("Authenticating with Minecraft services...")
                color: Theme.textSecondary
                Layout.alignment: Qt.AlignHCenter
            }
        }
        
        // Stage 3: Done
        ColumnLayout {
            visible: stage === 3
            Layout.fillWidth: true
            spacing: Theme.spacingS
            
            Label {
                text: "✓"
                font.pointSize: 48
                color: Theme.success
                Layout.alignment: Qt.AlignHCenter
            }
            
            Label {
                text: qsTr("Account added successfully!")
                font.bold: true
                color: Theme.success
                Layout.alignment: Qt.AlignHCenter
            }
        }
        
        Item { Layout.fillHeight: true }
    }
    
    Connections {
        target: vm
        
        function onLoginUrlReady(url, code) {
            loginUrl = url
            userCode = code
            stage = 1
        }
        
        function onLoginStarted() {
            stage = 2
        }
        
        function onLoginFinished(success, message) {
            if (success) {
                stage = 3
                closeTimer.start()
            } else {
                stage = 0
                errorDialog.message = message
                errorDialog.open()
            }
        }
    }
    
    Timer {
        id: closeTimer
        interval: 1500
        onTriggered: msaLoginDialog.accept()
    }
    
    Dialog {
        id: errorDialog
        title: qsTr("Login Error")
        modal: true
        standardButtons: Dialog.Ok
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        
        property string message: ""
        
        Label {
            text: errorDialog.message
            color: Theme.error
            wrapMode: Text.WordWrap
        }
    }
    
    onOpened: {
        stage = 0
        if (vm) vm.addMicrosoftAccount()
    }
    
    onRejected: {
        if (vm) vm.cancelLogin()
    }
}
