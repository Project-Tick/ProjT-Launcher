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

ScrollView {
    id: apiPage
    clip: true

    property var vm: ProjT.launcherSettingsVM

    ColumnLayout {
        width: apiPage.width - Theme.spacingL
        spacing: Theme.spacingM

        // Pastebin Service
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("&Pastebin Service")

            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS

                Label {
                    text: qsTr("Paste Service &Type")
                    color: ThemeColors.text
                }

                ComboBox {
                    id: pasteTypeComboBox
                    model: vm ? vm.pasteServiceTypes : []
                    currentIndex: vm ? vm.pasteServiceType : 0
                    onActivated: if (vm)
                        vm.pasteServiceType = currentIndex
                }

                Label {
                    text: qsTr("Base &URL")
                    color: ThemeColors.text
                }

                TextField {
                    id: baseURLEntry
                    Layout.fillWidth: true
                    placeholderText: qsTr("Use Default")
                    text: vm ? vm.pasteBaseUrl : ""
                    onTextChanged: if (vm)
                        vm.pasteBaseUrl = text
                }

                Label {
                    text: qsTr("Note: you probably want to change or clear the Base URL after changing the paste service type.")
                    color: ThemeColors.textSecondary
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }
            }
        }

        // Metadata Server
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Meta&data Server")

            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS

                Label {
                    text: qsTr("You can set this to a third-party metadata server to use patched libraries or other hacks.")
                    color: ThemeColors.text
                    textFormat: Text.RichText
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                TextField {
                    id: metaURL
                    Layout.fillWidth: true
                    placeholderText: qsTr("Use Default")
                    text: vm ? vm.metaUrl : ""
                    onTextChanged: if (vm)
                        vm.metaUrl = text
                }
            }
        }

        // Assets Server
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Assets Server")

            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS

                Label {
                    text: qsTr("You can set this to another server if you have problems with downloading assets.")
                    color: ThemeColors.text
                    textFormat: Text.RichText
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                TextField {
                    id: resourceURL
                    Layout.fillWidth: true
                    placeholderText: qsTr("Use Default")
                    text: vm ? vm.resourceUrl : ""
                    onTextChanged: if (vm)
                        vm.resourceUrl = text
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

                TextField {
                    id: userAgentLineEdit
                    Layout.fillWidth: true
                    placeholderText: qsTr("Use Default")
                    text: vm ? vm.userAgent : ""
                    onTextChanged: if (vm)
                        vm.userAgent = text
                }

                Label {
                    text: qsTr("Enter a custom User Agent here. The special string $LAUNCHER_VER will be replaced with the version of the launcher.")
                    color: ThemeColors.textSecondary
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }
            }
        }

        // API Keys
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("&API Keys")

            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS

                // Microsoft Authentication
                Label {
                    text: qsTr("&Microsoft Authentication")
                    color: ThemeColors.text
                    textFormat: Text.RichText
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                TextField {
                    id: msaClientID
                    Layout.fillWidth: true
                    placeholderText: qsTr("Use Default")
                    text: vm ? vm.msaClientId : ""
                    onTextChanged: if (vm)
                        vm.msaClientId = text
                }

                Label {
                    text: qsTr("Note: you probably don't need to set this if logging in via Microsoft Authentication already works.")
                    color: ThemeColors.textSecondary
                    textFormat: Text.RichText
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                Item {
                    height: 6
                }

                // Modrinth
                Label {
                    text: qsTr("Mod&rinth")
                    color: ThemeColors.text
                    textFormat: Text.RichText
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                TextField {
                    id: modrinthToken
                    Layout.fillWidth: true
                    placeholderText: qsTr("Use None")
                    text: vm ? vm.modrinthToken : ""
                    onTextChanged: if (vm)
                        vm.modrinthToken = text
                }

                Label {
                    text: "Note: you only need to set this to access private data. Read the <a href='https://docs.modrinth.com/api/#authentication'>documentation</a> for more information."
                    textFormat: Text.RichText
                    color: ThemeColors.textSecondary
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                    onLinkActivated: Qt.openUrlExternally(link)

                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.NoButton
                        cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                    }
                }

                Item {
                    height: 6
                }

                // CurseForge
                Label {
                    text: qsTr("&CurseForge")
                    color: ThemeColors.text
                    textFormat: Text.RichText
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                TextField {
                    id: flameKey
                    Layout.fillWidth: true
                    placeholderText: qsTr("Use Default")
                    text: vm ? vm.curseforgeApiKey : ""
                    onTextChanged: if (vm)
                        vm.curseforgeApiKey = text
                }

                Label {
                    text: qsTr("Note: you probably don't need to set this if CurseForge already works.")
                    color: ThemeColors.textSecondary
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                Item {
                    height: 6
                }

                // Technic
                Label {
                    text: qsTr("&Technic")
                    color: ThemeColors.text
                }

                TextField {
                    id: technicClientID
                    Layout.fillWidth: true
                    placeholderText: qsTr("Use Default")
                    text: vm ? vm.technicClientId : ""
                    onTextChanged: if (vm)
                        vm.technicClientId = text
                }

                Label {
                    text: qsTr("Note: you only need to set this to access private data.")
                    color: ThemeColors.textSecondary
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
