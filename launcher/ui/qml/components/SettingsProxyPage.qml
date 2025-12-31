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

ScrollView {
    id: proxyPage
    clip: true
    contentWidth: availableWidth

    property var vm: ProjT.launcherSettingsVM

    ColumnLayout {
        width: parent.width - ThemeColors.spacingL
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: ThemeColors.spacingM

        Label {
            text: qsTr("⚠️ These settings only apply to the launcher. Minecraft does not use them.")
            color: ThemeColors.warning
            font.weight: Font.DemiBold
            Layout.fillWidth: true
            visible: true 
        }

        SettingsSection {
            Layout.fillWidth: true
            title: qsTr("Proxy Configuration")
            iconSource: Theme.icon("proxy")

            ColumnLayout {
                spacing: ThemeColors.spacingM
                Layout.fillWidth: true

                // Type
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: ThemeColors.spacingS
                    Label { text: qsTr("Proxy Type"); color: ThemeColors.textTitle; font.weight: Font.DemiBold }
                    
                    ColumnLayout {
                        spacing: ThemeColors.spacingXS
                        RadioButton {
                            text: qsTr("Use System Settings")
                            checked: vm ? vm.proxyType === "default" : true
                            onCheckedChanged: if(vm && checked) vm.proxyType = "default"
                        }
                        RadioButton {
                            text: qsTr("No Proxy")
                            checked: vm ? vm.proxyType === "none" : false
                            onCheckedChanged: if(vm && checked) vm.proxyType = "none"
                        }
                        RadioButton {
                            id: proxySOCKS5Btn
                            text: qsTr("SOCKS5")
                            checked: vm ? vm.proxyType === "socks5" : false
                            onCheckedChanged: if(vm && checked) vm.proxyType = "socks5"
                        }
                        RadioButton {
                            id: proxyHTTPBtn
                            text: qsTr("HTTP")
                            checked: vm ? vm.proxyType === "http" : false
                            onCheckedChanged: if(vm && checked) vm.proxyType = "http"
                        }
                    }
                }

                Rectangle { height: 1; color: ThemeColors.separator; Layout.fillWidth: true; visible: proxySOCKS5Btn.checked || proxyHTTPBtn.checked }

                // Connection Details
                ColumnLayout {
                    visible: proxySOCKS5Btn.checked || proxyHTTPBtn.checked
                    Layout.fillWidth: true
                    spacing: ThemeColors.spacingS
                    
                    Label { text: qsTr("Connection Details"); color: ThemeColors.textTitle; font.weight: Font.DemiBold }
                    
                    RowLayout {
                        spacing: ThemeColors.spacingM
                        TextField {
                            id: proxyAddrEdit
                            Layout.fillWidth: true
                            placeholderText: "127.0.0.1"
                            text: vm ? vm.proxyHost : ""
                            onTextChanged: if(vm) vm.proxyHost = text
                        }
                        SpinBox {
                            id: proxyPortEdit
                            from: 1; to: 65535
                            value: vm ? vm.proxyPort : 8080
                            editable: true
                            onValueModified: if(vm) vm.proxyPort = value
                        }
                    }
                }

                // Authentication
                ColumnLayout {
                    visible: proxySOCKS5Btn.checked || proxyHTTPBtn.checked
                    Layout.fillWidth: true
                    spacing: ThemeColors.spacingS
                    
                    Label { text: qsTr("Authentication"); color: ThemeColors.textTitle; font.weight: Font.DemiBold }
                    
                    TextField {
                        id: proxyUserEdit
                        Layout.fillWidth: true
                        placeholderText: qsTr("Username")
                        text: vm ? vm.proxyUsername : ""
                        onTextChanged: if(vm) vm.proxyUsername = text
                    }
                    TextField {
                        id: proxyPassEdit
                        Layout.fillWidth: true
                        placeholderText: qsTr("Password")
                        echoMode: TextInput.Password
                        text: vm ? vm.proxyPassword : ""
                        onTextChanged: if(vm) vm.proxyPassword = text
                    }
                    Label {
                        text: qsTr("Credentials are stored in plain text in the config file.")
                        color: ThemeColors.error
                        font.pixelSize: 11
                    }
                }
            }
        }
        
        Item { height: ThemeColors.spacingL }
    }
}
