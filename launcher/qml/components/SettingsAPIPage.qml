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
import "../Theme.js" as Theme

ScrollView {
    id: apiPage
    clip: true
    
    property var vm: launcherSettingsVM
    
    // Pastebin service options
    property var pastebinServices: ["0x0.st", "paste.gg", "mclo.gs", "hastebin"]
    
    ColumnLayout {
        width: apiPage.width - Theme.spacingL
        spacing: Theme.spacingM
        
        // Pastebin
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Pastebin")
            
            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS
                
                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingM
                    
                    Label {
                        text: qsTr("Service:")
                        color: Theme.textPrimary
                    }
                    
                    ComboBox {
                        id: pastebinTypeCombo
                        Layout.fillWidth: true
                        model: pastebinServices
                        currentIndex: vm ? vm.pastebinType : 0
                        onActivated: if (vm) vm.pastebinType = currentIndex
                    }
                }
                
                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingM
                    
                    Label {
                        text: qsTr("Custom URL:")
                        color: Theme.textPrimary
                    }
                    
                    TextField {
                        id: pastebinUrlField
                        Layout.fillWidth: true
                        placeholderText: qsTr("Leave empty to use default")
                        text: vm ? vm.pastebinCustomUrl : ""
                        onTextChanged: if (vm) vm.pastebinCustomUrl = text
                    }
                }
            }
        }
        
        // Microsoft Authentication
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Microsoft Authentication")
            
            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS
                
                Label {
                    text: qsTr("These settings are for advanced users only. Misconfiguration may prevent login.")
                    color: Theme.warning
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }
                
                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingM
                    
                    Label {
                        text: qsTr("Client ID:")
                        color: Theme.textPrimary
                    }
                    
                    TextField {
                        id: msaClientIdField
                        Layout.fillWidth: true
                        placeholderText: qsTr("Leave empty to use default")
                        echoMode: TextInput.Password
                        text: vm ? vm.msaClientId : ""
                        onTextChanged: if (vm) vm.msaClientId = text
                    }
                }
            }
        }
        
        // Metadata Server
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Metadata Server")
            
            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS
                
                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingM
                    
                    Label {
                        text: qsTr("URL:")
                        color: Theme.textPrimary
                    }
                    
                    TextField {
                        id: metaUrlField
                        Layout.fillWidth: true
                        placeholderText: qsTr("http://meta.yongdohyun.org.tr/")
                        text: vm ? vm.metaUrl : ""
                        onTextChanged: if (vm) vm.metaUrl = text
                    }
                }
                
                Label {
                    text: qsTr("Change this only if you know what you're doing.")
                    color: Theme.textSecondary
                    font.pointSize: 9
                }
            }
        }
        
        // CurseForge
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("CurseForge")
            
            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS
                
                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingM
                    
                    Label {
                        text: qsTr("API Key:")
                        color: Theme.textPrimary
                    }
                    
                    TextField {
                        id: curseforgeKeyField
                        Layout.fillWidth: true
                        placeholderText: qsTr("Leave empty to use default")
                        echoMode: TextInput.Password
                        text: vm ? vm.curseforgeApiKey : ""
                        onTextChanged: if (vm) vm.curseforgeApiKey = text
                    }
                }
            }
        }
        
        // Modrinth
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Modrinth")
            
            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS
                
                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingM
                    
                    Label {
                        text: qsTr("API Token:")
                        color: Theme.textPrimary
                    }
                    
                    TextField {
                        id: modrinthTokenField
                        Layout.fillWidth: true
                        placeholderText: qsTr("Leave empty to use default")
                        echoMode: TextInput.Password
                        text: vm ? vm.modrinthToken : ""
                        onTextChanged: if (vm) vm.modrinthToken = text
                    }
                }
            }
        }
        
        // User Agent
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("User Agent")
            
            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS
                
                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingM
                    
                    Label {
                        text: qsTr("Override:")
                        color: Theme.textPrimary
                    }
                    
                    TextField {
                        id: userAgentField
                        Layout.fillWidth: true
                        placeholderText: qsTr("Leave empty to use default")
                        text: vm ? vm.userAgentOverride : ""
                        onTextChanged: if (vm) vm.userAgentOverride = text
                    }
                }
                
                Label {
                    text: qsTr("Custom user agent for network requests. Leave empty for default.")
                    color: Theme.textSecondary
                    font.pointSize: 9
                }
            }
        }
        
        Item { height: Theme.spacingL }
    }
}
