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
 *
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import ProjTLauncher 1.0
import "../Theme.js" as Theme

WindowDialog {
    id: blockedModsDialog
    title: qsTr("Blocked Mods")
    modal: true
    standardButtons: Dialog.Ok
    width: 550
    height: 450

    property var blockedMods: []
    property string instanceName: ""

    ColumnLayout {
        anchors.fill: parent
        spacing: Theme.spacingM

        // Warning header
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            radius: 8
            color: ThemeColors.warning
            opacity: 0.15

            RowLayout {
                anchors.fill: parent
                anchors.margins: Theme.spacingM
                spacing: Theme.spacingM

                Rectangle {
                    Layout.preferredWidth: 40
                    Layout.preferredHeight: 40
                    radius: 20
                    color: ThemeColors.warning

                    Label {
                        anchors.centerIn: parent
                        text: "!"
                        font.bold: true
                        font.pointSize: 18
                        color: "white"
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 2

                    Label {
                        text: qsTr("Some mods require manual download")
                        color: ThemeColors.warning
                        font.bold: true
                    }

                    Label {
                        text: qsTr("These mods cannot be downloaded automatically due to licensing restrictions.")
                        color: ThemeColors.textSecondary
                        font.pointSize: Theme.fontSizeSmall
                        wrapMode: Text.WordWrap
                    }
                }
            }
        }

        // Instructions
        Label {
            Layout.fillWidth: true
            text: qsTr("Please download the following mods manually and place them in the mods folder:")
            color: ThemeColors.text
            wrapMode: Text.WordWrap
        }

        // Blocked mods list
        Frame {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ListView {
                id: modsList
                anchors.fill: parent
                clip: true
                model: blockedMods
                spacing: 2

                delegate: Rectangle {
                    width: modsList.width
                    height: 56
                    radius: 4
                    color: index % 2 === 0 ? "transparent" : ThemeColors.backgroundAlt

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: Theme.spacingS
                        spacing: Theme.spacingM

                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2

                            Label {
                                text: modelData.name || qsTr("Unknown Mod")
                                color: ThemeColors.text
                                font.bold: true
                            }

                            Label {
                                text: modelData.version || ""
                                color: ThemeColors.textSecondary
                                font.pointSize: Theme.fontSizeSmall
                                visible: text.length > 0
                            }
                        }

                        Button {
                            text: qsTr("Open Page")
                            icon.name: "internet-web-browser"
                            visible: modelData.url && modelData.url.length > 0
                            onClicked: {
                                Qt.openUrlExternally(modelData.url);
                            }
                        }

                        Button {
                            text: qsTr("Copy URL")
                            icon.name: "edit-copy"
                            visible: modelData.url && modelData.url.length > 0
                            onClicked: {
                                if (ProjT) {
                                    ProjT.copyToClipboard(modelData.url);
                                }
                            }
                        }
                    }
                }

                ScrollBar.vertical: ScrollBar {}
            }

            Label {
                anchors.centerIn: parent
                text: qsTr("No blocked mods")
                color: ThemeColors.textSecondary
                visible: blockedMods.length === 0
            }
        }

        // Actions
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingM

            Button {
                text: qsTr("Open Mods Folder")
                icon.name: "folder-open"
                onClicked: {
                    if (ProjT && ProjT.instanceVM) {
                        ProjT.instanceVM.openModsFolder();
                    }
                }
            }

            Item {
                Layout.fillWidth: true
            }

            Label {
                text: qsTr("%1 mod(s) require manual download").arg(blockedMods.length)
                color: ThemeColors.textSecondary
                font.pointSize: Theme.fontSizeSmall
            }
        }
    }
}
