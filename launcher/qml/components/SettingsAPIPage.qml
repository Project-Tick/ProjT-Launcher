// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  API settings page
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme.js" as Theme

ScrollView {
    id: apiPage
    clip: true
    
    property var vm: launcherSettingsVM
    
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
                        Layout.fillWidth: true
                        model: ["0x0.st", "paste.gg", "mclo.gs", "hastebin"]
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
                        Layout.fillWidth: true
                        placeholderText: qsTr("Leave empty to use default")
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
                        Layout.fillWidth: true
                        placeholderText: qsTr("Leave empty to use default")
                        echoMode: TextInput.Password
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
                        Layout.fillWidth: true
                        placeholderText: qsTr("https://meta.prismlauncher.org/v1")
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
                        Layout.fillWidth: true
                        placeholderText: qsTr("Leave empty to use default")
                        echoMode: TextInput.Password
                    }
                }
            }
        }
        
        Item { height: Theme.spacingL }
    }
}
