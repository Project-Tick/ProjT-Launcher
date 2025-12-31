// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import ProjTLauncher 1.0
import "../Theme.js" as Theme
import "."

Rectangle {
    id: accountsPage
    color: "transparent"

    // Login dialog state
    property bool showLoginDialog: false
    property string loginUrl: ""
    property string loginCode: ""

    Connections {
        target: accountsVM
        function onLoginStarted() { showLoginDialog = true; }
        function onLoginFinished(success, message) {
            showLoginDialog = false;
            loginUrl = "";
            loginCode = "";
            if (!success) console.log("Login failed: " + message);
        }
        function onLoginUrlReady(url, code) {
            loginUrl = url;
            loginCode = code;
        }
    }

    ScrollView {
        anchors.fill: parent
        clip: true
        contentWidth: availableWidth
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

        Rectangle {
            width: parent.width
            implicitHeight: mainColumn.implicitHeight + 40
            color: "transparent"

            ColumnLayout {
                id: mainColumn
                width: Math.min(parent.width - 40, 700)
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: 20
                spacing: 16

                // === Header with Add Buttons ===
                Rectangle {
                    Layout.fillWidth: true
                    height: 80
                    radius: 12
                    color: ThemeColors.cardBackground
                    border.color: Qt.rgba(ThemeColors.cardBorder.r, ThemeColors.cardBorder.g, ThemeColors.cardBorder.b, 0.5)

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 20
                        spacing: 16

                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 4

                            Label {
                                text: qsTr("Minecraft Accounts")
                                color: ThemeColors.textTitle
                                font.pixelSize: 18
                                font.weight: Font.DemiBold
                            }
                            Label {
                                text: qsTr("Add and manage your Minecraft accounts")
                                color: ThemeColors.textSecondary
                                font.pixelSize: 13
                            }
                        }

                        ThemedButton {
                            text: qsTr("Add Microsoft")
                            primary: true
                            onClicked: accountsVM.addMicrosoftAccount()
                        }

                        ThemedButton {
                            text: qsTr("Add Offline")
                            onClicked: offlineDialog.open()
                        }
                    }
                }

                // === Account List ===
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: Math.max(300, accountList.contentHeight + 40)
                    radius: 12
                    color: ThemeColors.cardBackground
                    border.color: Qt.rgba(ThemeColors.cardBorder.r, ThemeColors.cardBorder.g, ThemeColors.cardBorder.b, 0.5)

                    ListView {
                        id: accountList
                        anchors.fill: parent
                        anchors.margins: 16
                        clip: true
                        model: accountsVM.model
                        spacing: 8

                        delegate: Rectangle {
                            id: accountDelegate
                            width: accountList.width
                            height: 72
                            radius: 10
                            color: mouseArea.containsMouse ? Qt.rgba(255, 255, 255, 0.05) : "transparent"
                            border.color: accountsVM.isAccountDefault(index) ? ThemeColors.accent : "transparent"
                            border.width: accountsVM.isAccountDefault(index) ? 1 : 0
                            
                            property var accountInfo: accountsVM.getAccountInfo(index)
                            property bool isDefault: accountsVM.isAccountDefault(index)

                            MouseArea {
                                id: mouseArea
                                anchors.fill: parent
                                hoverEnabled: true
                            }

                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 12
                                spacing: 16

                                // Avatar
                                Rectangle {
                                    width: 48
                                    height: 48
                                    radius: 24
                                    color: ThemeColors.bg1
                                    border.color: ThemeColors.border
                                    border.width: 1

                                    // Gradient overlay
                                    Rectangle {
                                        anchors.fill: parent
                                        radius: parent.radius
                                        gradient: Gradient {
                                            GradientStop { position: 0.0; color: Qt.rgba(255, 255, 255, 0.1) }
                                            GradientStop { position: 1.0; color: "transparent" }
                                        }
                                    }

                                    Text {
                                        anchors.centerIn: parent
                                        text: {
                                            var name = accountsVM.getAccountName(index) || "?";
                                            return name.charAt(0).toUpperCase();
                                        }
                                        color: ThemeColors.accent
                                        font.pixelSize: 20
                                        font.bold: true
                                    }
                                    
                                    // Status Indicator
                                    Rectangle {
                                        width: 14
                                        height: 14
                                        radius: 7
                                        anchors.right: parent.right
                                        anchors.bottom: parent.bottom
                                        anchors.margins: -2
                                        color: {
                                            var info = accountDelegate.accountInfo;
                                            if (info && info.status === "online") return ThemeColors.success;
                                            if (info && (info.status === "expired" || info.status === "error")) return ThemeColors.error;
                                            return ThemeColors.textSecondary;
                                        }
                                        border.color: ThemeColors.cardBackground
                                        border.width: 2
                                    }
                                }

                                // Details
                                ColumnLayout {
                                    Layout.fillWidth: true
                                    spacing: 4
                                    
                                    RowLayout {
                                        spacing: 8
                                        Label {
                                            text: accountsVM.getAccountName(index) || qsTr("Unknown")
                                            color: ThemeColors.textTitle
                                            font.weight: Font.DemiBold
                                            font.pixelSize: 15
                                        }
                                        
                                        Rectangle {
                                            visible: accountDelegate.isDefault
                                            width: activeLabel.width + 16
                                            height: 22
                                            radius: 11
                                            color: Qt.rgba(ThemeColors.accent.r, ThemeColors.accent.g, ThemeColors.accent.b, 0.2)
                                            border.color: ThemeColors.accent
                                            border.width: 1
                                            
                                            Label {
                                                id: activeLabel
                                                anchors.centerIn: parent
                                                text: qsTr("Active")
                                                color: ThemeColors.accent
                                                font.pixelSize: 11
                                                font.weight: Font.Medium
                                            }
                                        }
                                    }

                                    Label {
                                        text: {
                                            var type = accountsVM.getAccountType(index);
                                            return type === "msa" ? qsTr("Microsoft Account") : 
                                                   (type === "offline" ? qsTr("Offline Account") : type);
                                        }
                                        color: ThemeColors.textSecondary
                                        font.pixelSize: 12
                                    }
                                }

                                // Actions
                                RowLayout {
                                    visible: mouseArea.containsMouse || accountDelegate.activeFocus
                                    spacing: 8
                                    
                                    ThemedButton {
                                        text: qsTr("Use")
                                        visible: !accountDelegate.isDefault
                                        size: "small"
                                        primary: true
                                        onClicked: accountsVM.setDefaultAccount(index)
                                    }
                                    
                                    ThemedButton {
                                        text: "🔄"
                                        visible: accountsVM.getAccountType(index) !== "offline"
                                        enabled: !accountsVM.isActive
                                        size: "small"
                                        flatStyle: true
                                        ToolTip.visible: hovered
                                        ToolTip.text: qsTr("Refresh Session")
                                        onClicked: accountsVM.refreshAccount(index)
                                    }
                                    
                                    ThemedButton {
                                        text: "🗑️"
                                        size: "small"
                                        flatStyle: true
                                        danger: true
                                        ToolTip.visible: hovered
                                        ToolTip.text: qsTr("Remove Account")
                                        onClicked: {
                                            removeConfirmDialog.accountIndex = index;
                                            removeConfirmDialog.accountName = accountsVM.getAccountName(index);
                                            removeConfirmDialog.open();
                                        }
                                    }
                                }
                            }
                        }

                        ScrollBar.vertical: ScrollBar {}
                    }

                    // Empty State
                    ColumnLayout {
                        anchors.centerIn: parent
                        visible: !accountsVM.hasAccounts
                        spacing: 16

                        Rectangle {
                            Layout.alignment: Qt.AlignHCenter
                            width: 80
                            height: 80
                            radius: 40
                            color: Qt.rgba(ThemeColors.accent.r, ThemeColors.accent.g, ThemeColors.accent.b, 0.1)

                            Text {
                                anchors.centerIn: parent
                                text: "👤"
                                font.pixelSize: 36
                            }
                        }
                        
                        Label {
                            Layout.alignment: Qt.AlignHCenter
                            text: qsTr("No accounts added")
                            color: ThemeColors.textTitle
                            font.pixelSize: 18
                            font.weight: Font.DemiBold
                        }
                        
                        Label {
                            Layout.alignment: Qt.AlignHCenter
                            text: qsTr("Add a Microsoft account to start playing Minecraft")
                            color: ThemeColors.textSecondary
                            font.pixelSize: 13
                        }
                        
                        ThemedButton {
                            Layout.alignment: Qt.AlignHCenter
                            text: qsTr("Add Microsoft Account")
                            primary: true
                            onClicked: accountsVM.addMicrosoftAccount()
                        }
                    }

                    // Loading Overlay
                    Rectangle {
                        anchors.fill: parent
                        radius: parent.radius
                        color: Qt.rgba(0, 0, 0, 0.6)
                        visible: accountsVM.isActive
                        
                        ColumnLayout {
                            anchors.centerIn: parent
                            spacing: 12
                            
                            BusyIndicator { 
                                Layout.alignment: Qt.AlignHCenter
                                running: parent.parent.visible 
                            }
                            
                            Label {
                                Layout.alignment: Qt.AlignHCenter
                                text: qsTr("Please wait...")
                                color: ThemeColors.text
                                font.pixelSize: 13
                            }
                        }
                    }
                }
                
                // Security Note
                Rectangle {
                    Layout.fillWidth: true
                    height: securityText.implicitHeight + 24
                    radius: 8
                    color: Qt.rgba(ThemeColors.success.r, ThemeColors.success.g, ThemeColors.success.b, 0.1)
                    border.color: Qt.rgba(ThemeColors.success.r, ThemeColors.success.g, ThemeColors.success.b, 0.3)
                    
                    Label {
                        id: securityText
                        anchors.fill: parent
                        anchors.margins: 12
                        horizontalAlignment: Text.AlignHCenter
                        text: "🔒 " + qsTr("Account credentials are stored securely in your system's keychain")
                        color: ThemeColors.success
                        font.pixelSize: 12
                    }
                }

                Item { height: 20 }
            }
        }
    }

    // Offline Dialog
    Dialog {
        id: offlineDialog
        title: qsTr("Add Offline Account")
        standardButtons: Dialog.Ok | Dialog.Cancel
        anchors.centerIn: parent
        modal: true
        parent: Overlay.overlay
        width: 400
        
        background: Rectangle {
            color: ThemeColors.surface
            border.color: ThemeColors.border
            radius: 12
        }

        header: Rectangle {
            width: parent.width
            height: 50
            color: "transparent"
            
            Label {
                anchors.left: parent.left
                anchors.leftMargin: 20
                anchors.verticalCenter: parent.verticalCenter
                text: offlineDialog.title
                color: ThemeColors.textTitle
                font.pixelSize: 16
                font.weight: Font.DemiBold
            }
        }

        ColumnLayout {
            width: parent.width
            spacing: 16
            
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 8
                
                Label { 
                    text: qsTr("Username")
                    color: ThemeColors.text
                    font.pixelSize: 13
                }
                
                TextField {
                    id: offlineUsernameField
                    Layout.fillWidth: true
                    placeholderText: qsTr("Steve")
                    selectByMouse: true
                }
            }
            
            Rectangle {
                Layout.fillWidth: true
                height: warningLabel.implicitHeight + 16
                radius: 8
                color: Qt.rgba(ThemeColors.warning.r, ThemeColors.warning.g, ThemeColors.warning.b, 0.1)
                border.color: Qt.rgba(ThemeColors.warning.r, ThemeColors.warning.g, ThemeColors.warning.b, 0.3)

                Label {
                    id: warningLabel
                    anchors.fill: parent
                    anchors.margins: 8
                    text: "⚠️ " + qsTr("Offline accounts cannot verify game ownership or access online servers")
                    color: ThemeColors.warning
                    font.pixelSize: 12
                    wrapMode: Text.WordWrap
                }
            }
        }
        
        onAccepted: {
            if (offlineUsernameField.text.length > 0) 
                accountsVM.addOfflineAccount(offlineUsernameField.text);
            offlineUsernameField.text = "";
        }
        onRejected: offlineUsernameField.text = ""
    }

    // Remove Dialog
    Dialog {
        id: removeConfirmDialog
        title: qsTr("Remove Account")
        standardButtons: Dialog.Yes | Dialog.No
        anchors.centerIn: parent
        modal: true
        parent: Overlay.overlay
        width: 400
        
        property int accountIndex: -1
        property string accountName: ""
        
        background: Rectangle {
            color: ThemeColors.surface
            border.color: ThemeColors.border
            radius: 12
        }

        header: Rectangle {
            width: parent.width
            height: 50
            color: "transparent"
            
            RowLayout {
                anchors.left: parent.left
                anchors.leftMargin: 20
                anchors.verticalCenter: parent.verticalCenter
                spacing: 8
                
                Text {
                    text: "⚠️"
                    font.pixelSize: 18
                }
                
                Label {
                    text: removeConfirmDialog.title
                    color: ThemeColors.textTitle
                    font.pixelSize: 16
                    font.weight: Font.DemiBold
                }
            }
        }

        ColumnLayout {
            width: parent.width
            spacing: 12
            
            Label {
                text: qsTr("Are you sure you want to remove '%1'?").arg(removeConfirmDialog.accountName)
                color: ThemeColors.text
                font.pixelSize: 14
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }
            
            Label {
                text: qsTr("This action cannot be undone.")
                color: ThemeColors.error
                font.pixelSize: 12
            }
        }
        
        onAccepted: if (accountIndex >= 0) accountsVM.removeAccount(accountIndex)
    }

    // Microsoft Login Dialog
    Dialog {
        id: loginDialog
        title: qsTr("Microsoft Login")
        standardButtons: Dialog.Cancel
        anchors.centerIn: parent
        modal: true
        visible: showLoginDialog && loginUrl.length > 0
        parent: Overlay.overlay
        width: 500
        
        background: Rectangle {
            color: ThemeColors.surface
            border.color: ThemeColors.border
            radius: 12
        }

        header: Rectangle {
            width: parent.width
            height: 60
            color: Qt.rgba(ThemeColors.accent.r, ThemeColors.accent.g, ThemeColors.accent.b, 0.1)
            radius: 12
            
            Rectangle {
                anchors.bottom: parent.bottom
                width: parent.width
                height: parent.radius
                color: parent.color
            }
            
            RowLayout {
                anchors.left: parent.left
                anchors.leftMargin: 20
                anchors.verticalCenter: parent.verticalCenter
                spacing: 12
                
                Rectangle {
                    width: 32
                    height: 32
                    radius: 8
                    color: ThemeColors.accent
                    
                    Text {
                        anchors.centerIn: parent
                        text: "🔐"
                        font.pixelSize: 16
                    }
                }
                
                Label {
                    text: qsTr("Sign in with Microsoft")
                    color: ThemeColors.textTitle
                    font.pixelSize: 18
                    font.weight: Font.DemiBold
                }
            }
        }

        ColumnLayout {
            width: parent.width
            spacing: 20

            // Step 1
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 8
                
                RowLayout {
                    spacing: 8
                    
                    Rectangle {
                        width: 24
                        height: 24
                        radius: 12
                        color: ThemeColors.accent
                        
                        Label {
                            anchors.centerIn: parent
                            text: "1"
                            color: "#FFFFFF"
                            font.pixelSize: 12
                            font.bold: true
                        }
                    }
                    
                    Label {
                        text: qsTr("Visit this URL in your browser:")
                        color: ThemeColors.text
                        font.pixelSize: 14
                    }
                }
                
                Rectangle {
                    Layout.fillWidth: true
                    height: 44
                    radius: 8
                    color: ThemeColors.bg1
                    border.color: ThemeColors.border
                    
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 8
                        spacing: 8
                        
                        Text {
                            Layout.fillWidth: true
                            text: loginUrl
                            color: ThemeColors.accent
                            font.pixelSize: 12
                            elide: Text.ElideMiddle
                        }
                        
                        ThemedButton {
                            text: qsTr("Copy")
                            size: "small"
                            onClicked: {
                                // Copy to clipboard would go here
                            }
                        }
                    }
                }
            }

            // Step 2
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 8
                visible: loginCode.length > 0
                
                RowLayout {
                    spacing: 8
                    
                    Rectangle {
                        width: 24
                        height: 24
                        radius: 12
                        color: ThemeColors.accent
                        
                        Label {
                            anchors.centerIn: parent
                            text: "2"
                            color: "#FFFFFF"
                            font.pixelSize: 12
                            font.bold: true
                        }
                    }
                    
                    Label {
                        text: qsTr("Enter this code when prompted:")
                        color: ThemeColors.text
                        font.pixelSize: 14
                    }
                }
                
                Rectangle {
                    Layout.fillWidth: true
                    height: 64
                    radius: 12
                    color: ThemeColors.bg1
                    border.color: ThemeColors.accent
                    border.width: 2
                    
                    Text {
                        anchors.centerIn: parent
                        text: loginCode
                        font.bold: true
                        font.pixelSize: 32
                        font.letterSpacing: 6
                        color: ThemeColors.textTitle
                    }
                }
            }

            // Open Button
            ThemedButton {
                Layout.alignment: Qt.AlignHCenter
                text: qsTr("Open Login Page")
                primary: true
                onClicked: Qt.openUrlExternally(loginUrl)
            }

            // Waiting indicator
            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                spacing: 12
                
                BusyIndicator { running: true; implicitWidth: 24; implicitHeight: 24 }
                
                Label {
                    text: qsTr("Waiting for authentication...")
                    color: ThemeColors.textSecondary
                    font.italic: true
                    font.pixelSize: 13
                }
            }
        }
        
        onRejected: accountsVM.cancelCurrentLogin()
    }
}
