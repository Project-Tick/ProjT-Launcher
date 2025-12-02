// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  Proxy settings page
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme.js" as Theme

ScrollView {
    id: proxyPage
    clip: true
    
    property var vm: launcherSettingsVM
    
    ColumnLayout {
        width: proxyPage.width - Theme.spacingL
        spacing: Theme.spacingM
        
        // Proxy Type
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Proxy Type")
            
            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS
                
                RadioButton {
                    id: noProxyRadio
                    text: qsTr("No proxy")
                    checked: vm ? vm.proxyType === "none" : true
                    onCheckedChanged: if (vm && checked) vm.proxyType = "none"
                }
                
                RadioButton {
                    id: systemProxyRadio
                    text: qsTr("Use system proxy settings")
                    checked: vm ? vm.proxyType === "system" : false
                    onCheckedChanged: if (vm && checked) vm.proxyType = "system"
                }
                
                RadioButton {
                    id: httpProxyRadio
                    text: qsTr("HTTP proxy")
                    checked: vm ? vm.proxyType === "http" : false
                    onCheckedChanged: if (vm && checked) vm.proxyType = "http"
                }
                
                RadioButton {
                    id: socks5ProxyRadio
                    text: qsTr("SOCKS5 proxy")
                    checked: vm ? vm.proxyType === "socks5" : false
                    onCheckedChanged: if (vm && checked) vm.proxyType = "socks5"
                }
            }
        }
        
        // Proxy Settings
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Proxy Settings")
            enabled: httpProxyRadio.checked || socks5ProxyRadio.checked
            
            GridLayout {
                anchors.fill: parent
                columns: 2
                rowSpacing: Theme.spacingS
                columnSpacing: Theme.spacingM
                
                Label {
                    text: qsTr("Address:")
                    color: Theme.textPrimary
                }
                
                TextField {
                    id: proxyHostField
                    Layout.fillWidth: true
                    placeholderText: qsTr("proxy.example.com")
                    text: vm ? vm.proxyHost : ""
                    onTextChanged: if (vm) vm.proxyHost = text
                }
                
                Label {
                    text: qsTr("Port:")
                    color: Theme.textPrimary
                }
                
                SpinBox {
                    id: proxyPortSpin
                    from: 1
                    to: 65535
                    value: vm ? vm.proxyPort : 8080
                    editable: true
                    onValueModified: if (vm) vm.proxyPort = value
                }
                
                Label {
                    text: qsTr("Username:")
                    color: Theme.textPrimary
                }
                
                TextField {
                    id: proxyUserField
                    Layout.fillWidth: true
                    placeholderText: qsTr("Optional")
                    text: vm ? vm.proxyUsername : ""
                    onTextChanged: if (vm) vm.proxyUsername = text
                }
                
                Label {
                    text: qsTr("Password:")
                    color: Theme.textPrimary
                }
                
                TextField {
                    id: proxyPassField
                    Layout.fillWidth: true
                    placeholderText: qsTr("Optional")
                    echoMode: TextInput.Password
                    text: vm ? vm.proxyPassword : ""
                    onTextChanged: if (vm) vm.proxyPassword = text
                }
            }
        }
        
        // Info
        Label {
            text: qsTr("Proxy settings apply to all network connections made by the launcher.")
            color: Theme.textSecondary
            font.pointSize: 9
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }
        
        Item { height: Theme.spacingL }
    }
}
