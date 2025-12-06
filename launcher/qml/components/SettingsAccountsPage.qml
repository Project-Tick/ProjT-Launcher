// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  This file is part of ProjT Launcher and is licensed under
 *  the GNU General Public License version 3 or later.
 *
 *  If this file includes work from previous open-source projects,
 *  their original copyright and license notices are preserved below.
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import ProjTLauncher 1.0
import "../Theme.js" as Theme

Rectangle {
    id: accountsPage
    color: ThemeColors.background
    
    // Login dialog state
    property bool showLoginDialog: false
    property string loginUrl: ""
    property string loginCode: ""
    
    Connections {
        target: accountsVM
        
        function onLoginStarted() {
            showLoginDialog = true
        }
        
        function onLoginFinished(success, message) {
            showLoginDialog = false
            loginUrl = ""
            loginCode = ""
            if (!success) {
                console.log("Login failed: " + message)
            }
        }
        
        function onLoginUrlReady(url, code) {
            loginUrl = url
            loginCode = code
        }
    }
    
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
                color: ThemeColors.text
            }
            
            Item { Layout.fillWidth: true }
            
            Button {
                text: qsTr("Add Microsoft")
                icon.name: "list-add"
                onClicked: {
                    accountsVM.addMicrosoftAccount()
                }
            }
            
            Button {
                text: qsTr("Add Offline")
                icon.name: "list-add"
                onClicked: {
                    offlineDialog.open()
                }
            }
            
            Button {
                text: qsTr("Refresh All")
                icon.name: "view-refresh"
                enabled: accountsVM.hasAccounts && !accountsVM.isActive
                onClicked: {
                    accountsVM.refreshAllAccounts()
                }
            }
        }
        
        // Account list
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: ThemeColors.surface
            border.color: ThemeColors.border
            border.width: 1
            radius: Theme.radiusS
            
            ListView {
                id: accountList
                anchors.fill: parent
                anchors.margins: 1
                clip: true
                
                model: accountsVM.model
                
                delegate: ItemDelegate {
                    id: accountDelegate
                    width: accountList.width
                    height: 60
                    
                    property var accountInfo: accountsVM.getAccountInfo(index)
                    property bool isDefault: accountsVM.isAccountDefault(index)
                    
                    background: Rectangle {
                        color: accountDelegate.hovered ? ThemeColors.surfaceHover : "transparent"
                    }
                    
                    contentItem: RowLayout {
                        spacing: Theme.spacingM
                        
                        // Avatar placeholder
                        Rectangle {
                            Layout.preferredWidth: 40
                            Layout.preferredHeight: 40
                            radius: 4
                            color: ThemeColors.border
                            
                            Label {
                                anchors.centerIn: parent
                                text: "👤"
                                font.pointSize: 20
                            }
                        }
                        
                        // Name and type
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2
                            
                            RowLayout {
                                spacing: Theme.spacingS
                                
                                Label {
                                    text: accountsVM.getAccountName(index) || qsTr("Unknown")
                                    color: ThemeColors.text
                                    font.bold: true
                                }
                                
                                Rectangle {
                                    visible: accountDelegate.isDefault
                                    width: defaultLabel.width + 8
                                    height: defaultLabel.height + 4
                                    radius: 2
                                    color: ThemeColors.success
                                    
                                    Label {
                                        id: defaultLabel
                                        anchors.centerIn: parent
                                        text: qsTr("Default")
                                        color: "white"
                                        font.pointSize: 8
                                    }
                                }
                            }
                            
                            Label {
                                text: {
                                    var type = accountsVM.getAccountType(index)
                                    if (type === "msa") return qsTr("Microsoft Account")
                                    if (type === "offline") return qsTr("Offline Account")
                                    return type
                                }
                                color: ThemeColors.textSecondary
                                font.pointSize: 9
                            }
                        }
                        
                        // Status
                        Label {
                            text: accountsVM.getAccountStatus(index)
                            color: {
                                var info = accountDelegate.accountInfo
                                if (info && info.status === "online") return ThemeColors.success
                                if (info && (info.status === "expired" || info.status === "error")) return ThemeColors.error
                                return ThemeColors.textSecondary
                            }
                            font.pointSize: 9
                        }
                        
                        // Actions
                        Button {
                            text: qsTr("Set Default")
                            visible: !accountDelegate.isDefault
                            onClicked: {
                                accountsVM.setDefaultAccount(index)
                            }
                        }
                        
                        Button {
                            text: qsTr("Refresh")
                            visible: accountsVM.getAccountType(index) !== "offline"
                            enabled: !accountsVM.isActive
                            onClicked: {
                                accountsVM.refreshAccount(index)
                            }
                        }
                        
                        Button {
                            text: qsTr("Remove")
                            onClicked: {
                                removeConfirmDialog.accountIndex = index
                                removeConfirmDialog.accountName = accountsVM.getAccountName(index)
                                removeConfirmDialog.open()
                            }
                        }
                    }
                }
                
                ScrollBar.vertical: ScrollBar {}
            }
            
            // Empty state
            ColumnLayout {
                anchors.centerIn: parent
                visible: !accountsVM.hasAccounts
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
                    color: ThemeColors.textSecondary
                    font.pointSize: 12
                }
                
                Label {
                    Layout.alignment: Qt.AlignHCenter
                    text: qsTr("Add a Microsoft account to play Minecraft")
                    color: ThemeColors.textSecondary
                }
                
                Button {
                    Layout.alignment: Qt.AlignHCenter
                    text: qsTr("Add Microsoft Account")
                    onClicked: {
                        accountsVM.addMicrosoftAccount()
                    }
                }
            }
            
            // Loading overlay
            Rectangle {
                anchors.fill: parent
                color: Qt.rgba(0, 0, 0, 0.5)
                visible: accountsVM.isActive
                
                BusyIndicator {
                    anchors.centerIn: parent
                    running: parent.visible
                }
            }
        }
        
        // Info
        Label {
            text: qsTr("Note: Account data is stored securely using system keychain.")
            color: ThemeColors.textSecondary
            font.pointSize: 9
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }
    }
    
    // Offline account dialog
    Dialog {
        id: offlineDialog
        title: qsTr("Add Offline Account")
        standardButtons: Dialog.Ok | Dialog.Cancel
        anchors.centerIn: parent
        modal: true
        
        ColumnLayout {
            spacing: Theme.spacingM
            
            Label {
                text: qsTr("Username:")
                color: ThemeColors.text
            }
            
            TextField {
                id: offlineUsernameField
                Layout.preferredWidth: 300
                placeholderText: qsTr("Enter username")
            }
            
            Label {
                text: qsTr("Offline accounts cannot access online servers.")
                color: ThemeColors.textSecondary
                font.pointSize: 9
                wrapMode: Text.WordWrap
            }
        }
        
        onAccepted: {
            if (offlineUsernameField.text.length > 0) {
                accountsVM.addOfflineAccount(offlineUsernameField.text)
            }
            offlineUsernameField.text = ""
        }
        
        onRejected: {
            offlineUsernameField.text = ""
        }
    }
    
    // Remove confirmation dialog
    Dialog {
        id: removeConfirmDialog
        title: qsTr("Remove Account")
        standardButtons: Dialog.Yes | Dialog.No
        anchors.centerIn: parent
        modal: true
        
        property int accountIndex: -1
        property string accountName: ""
        
        Label {
            text: qsTr("Are you sure you want to remove '%1'?").arg(removeConfirmDialog.accountName)
            color: ThemeColors.text
            wrapMode: Text.WordWrap
        }
        
        onAccepted: {
            if (accountIndex >= 0) {
                accountsVM.removeAccount(accountIndex)
            }
        }
    }
    
    // Login dialog (for Microsoft login URL)
    Dialog {
        id: loginDialog
        title: qsTr("Microsoft Login")
        standardButtons: Dialog.Cancel
        anchors.centerIn: parent
        modal: true
        visible: showLoginDialog && loginUrl.length > 0
        
        ColumnLayout {
            spacing: Theme.spacingM
            
            Label {
                text: qsTr("Please visit the following URL to complete login:")
                color: ThemeColors.text
                wrapMode: Text.WordWrap
                Layout.maximumWidth: 400
            }
            
            TextField {
                id: urlField
                text: loginUrl
                readOnly: true
                Layout.preferredWidth: 400
                selectByMouse: true
            }
            
            Label {
                visible: loginCode.length > 0
                text: qsTr("Enter this code: %1").arg(loginCode)
                color: ThemeColors.text
                font.bold: true
                font.pointSize: 14
                Layout.alignment: Qt.AlignHCenter
            }
            
            Button {
                text: qsTr("Open in Browser")
                Layout.alignment: Qt.AlignHCenter
                onClicked: {
                    Qt.openUrlExternally(loginUrl)
                }
            }
            
            BusyIndicator {
                Layout.alignment: Qt.AlignHCenter
                running: showLoginDialog
            }
            
            Label {
                text: qsTr("Waiting for login...")
                color: ThemeColors.textSecondary
                Layout.alignment: Qt.AlignHCenter
            }
        }
        
        onRejected: {
            accountsVM.cancelCurrentLogin()
        }
    }
}