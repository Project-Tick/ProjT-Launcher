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
    id: apiPage
    clip: true
    contentWidth: availableWidth

    property var vm: ProjT.launcherSettingsVM

    ColumnLayout {
        width: parent.width - ThemeColors.spacingL
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: ThemeColors.spacingM

        // Pastebin Service
        SettingsSection {
            Layout.fillWidth: true
            title: qsTr("Paste Service")
            iconSource: Theme.icon("news")

            ColumnLayout {
                spacing: ThemeColors.spacingS
                Layout.fillWidth: true

                Label { text: qsTr("Service Type"); color: ThemeColors.textTitle; font.weight: Font.DemiBold }
                ComboBox {
                    id: pasteTypeComboBox
                    Layout.fillWidth: true
                    model: vm ? vm.pasteServiceTypes : []
                    currentIndex: vm ? vm.pasteServiceType : 0
                    onActivated: if(vm) vm.pasteServiceType = currentIndex
                }

                Label { text: qsTr("Base URL"); color: ThemeColors.textTitle; font.weight: Font.DemiBold }
                TextField {
                    id: baseURLEntry
                    Layout.fillWidth: true
                    placeholderText: qsTr("Use Default")
                    text: vm ? vm.pasteBaseUrl : ""
                    onTextChanged: if(vm) vm.pasteBaseUrl = text
                }
                Label {
                    text: qsTr("Note: Update the Base URL if you change the service type.")
                    color: ThemeColors.textSecondary
                    font.pixelSize: 11
                }
            }
        }

        // Servers (Metadata & Assets)
        SettingsSection {
            Layout.fillWidth: true
            title: qsTr("Custom Servers")
            iconSource: Theme.icon("server")

            ColumnLayout {
                spacing: ThemeColors.spacingM
                Layout.fillWidth: true

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: ThemeColors.spacingS
                    Label { text: qsTr("Metadata Server"); color: ThemeColors.textTitle; font.weight: Font.DemiBold }
                     TextField {
                        id: metaURL
                        Layout.fillWidth: true
                        placeholderText: qsTr("Use Default")
                        text: vm ? vm.metaUrl : ""
                        onTextChanged: if(vm) vm.metaUrl = text
                    }
                    Label {
                        text: qsTr("Used for patched libraries or custom launcher metadata.")
                        color: ThemeColors.textSecondary
                        font.pixelSize: 11
                    }
                }

                 ColumnLayout {
                    Layout.fillWidth: true
                    spacing: ThemeColors.spacingS
                    Label { text: qsTr("Assets Server"); color: ThemeColors.textTitle; font.weight: Font.DemiBold }
                     TextField {
                        id: resourceURL
                        Layout.fillWidth: true
                        placeholderText: qsTr("Use Default")
                        text: vm ? vm.resourceUrl : ""
                        onTextChanged: if(vm) vm.resourceUrl = text
                    }
                    Label {
                        text: qsTr("Alternative server for downloading game assets.")
                        color: ThemeColors.textSecondary
                        font.pixelSize: 11
                    }
                }
            }
        }

        // API Keys
        SettingsSection {
            Layout.fillWidth: true
            title: qsTr("API Keys")
            iconSource: Theme.icon("accounts")

            ColumnLayout {
                spacing: ThemeColors.spacingM
                Layout.fillWidth: true

                // Microsoft
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: ThemeColors.spacingS
                    Label { text: qsTr("Microsoft Authentication Client ID"); color: ThemeColors.textTitle; font.weight: Font.DemiBold }
                    TextField {
                        id: msaClientID
                        Layout.fillWidth: true
                        placeholderText: qsTr("Use Default")
                        text: vm ? vm.msaClientId : ""
                        onTextChanged: if(vm) vm.msaClientId = text
                    }
                }

                Rectangle { height: 1; color: ThemeColors.separator; Layout.fillWidth: true }

                // Modrinth
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: ThemeColors.spacingS
                    Label { text: qsTr("Modrinth Token"); color: ThemeColors.textTitle; font.weight: Font.DemiBold }
                    TextField {
                        id: modrinthToken
                        Layout.fillWidth: true
                        placeholderText: qsTr("None")
                        text: vm ? vm.modrinthToken : ""
                        onTextChanged: if(vm) vm.modrinthToken = text
                    }
                    Text {
                        text: "Required for accessing private Modrinth data. <a href='https://docs.modrinth.com/api/#authentication'>Documentation</a>"
                        color: ThemeColors.textSecondary
                        font.pixelSize: 11
                        onLinkActivated: Qt.openUrlExternally(link)
                         MouseArea { anchors.fill: parent; acceptedButtons: Qt.NoButton; cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor }
                    }
                }

                Rectangle { height: 1; color: ThemeColors.separator; Layout.fillWidth: true }

                // CurseForge
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: ThemeColors.spacingS
                    Label { text: qsTr("CurseForge API Key"); color: ThemeColors.textTitle; font.weight: Font.DemiBold }
                    TextField {
                        id: flameKey
                        Layout.fillWidth: true
                        placeholderText: qsTr("Use Default")
                        text: vm ? vm.curseforgeApiKey : ""
                        onTextChanged: if(vm) vm.curseforgeApiKey = text
                    }
                }
                
                Rectangle { height: 1; color: ThemeColors.separator; Layout.fillWidth: true }

                // Technic
                 ColumnLayout {
                    Layout.fillWidth: true
                    spacing: ThemeColors.spacingS
                    Label { text: qsTr("Technic Client ID"); color: ThemeColors.textTitle; font.weight: Font.DemiBold }
                    TextField {
                        id: technicClientID
                        Layout.fillWidth: true
                        placeholderText: qsTr("Use Default")
                        text: vm ? vm.technicClientId : ""
                        onTextChanged: if(vm) vm.technicClientId = text
                    }
                }
            }
        }
        
        // User Agent
        SettingsSection {
            Layout.fillWidth: true
            title: qsTr("Network")
            iconSource: Theme.icon("proxy")
             ColumnLayout {
                    Layout.fillWidth: true
                    spacing: ThemeColors.spacingS
                    Label { text: qsTr("User Agent"); color: ThemeColors.textTitle; font.weight: Font.DemiBold }
                    TextField {
                        id: userAgentLineEdit
                        Layout.fillWidth: true
                        placeholderText: qsTr("Use Default")
                        text: vm ? vm.userAgent : ""
                        onTextChanged: if(vm) vm.userAgent = text
                    }
                    Label {
                        text: qsTr("Custom User Agent string. Use $LAUNCHER_VER for version.")
                        color: ThemeColors.textSecondary
                        font.pixelSize: 11
                    }
                }
        }
        
        Item { height: ThemeColors.spacingL }
    }
}
