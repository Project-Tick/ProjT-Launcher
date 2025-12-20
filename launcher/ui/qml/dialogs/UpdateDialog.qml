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
import "../components"

WindowDialog {
    id: updateDialog
    title: qsTr("Update Available")
    modal: true
    width: 636
    height: 352
    standardButtons: Dialog.NoButton

    property string currentVersion: ""
    property string newVersion: ""
    property string releaseNotes: ""
    property string downloadUrl: ""
    property var vm: null

    signal skipVersion
    signal remindLater
    signal installUpdate

    ColumnLayout {
        anchors.fill: parent
        spacing: Theme.spacingM

        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingM

            // Icon
            ColumnLayout {
                Layout.alignment: Qt.AlignTop

                Image {
                    Layout.preferredWidth: 64
                    Layout.preferredHeight: 64
                    source: "qrc:/icons/multimc/scalable/instances/default.svg"
                    fillMode: Image.PreserveAspectFit
                }

                Item {
                    Layout.fillHeight: true
                }
            }

            // Content
            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.margins: 9
                spacing: Theme.spacingS

                Label {
                    text: qsTr("A new version is available!")
                    font.bold: true
                    font.pointSize: 11
                    color: ThemeColors.text
                }

                Label {
                    text: qsTr("Version %1 is now available - you have %2. Would you like to download it now?").arg(newVersion).arg(currentVersion)
                    color: ThemeColors.text
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                Label {
                    text: qsTr("Release Notes:")
                    font.bold: true
                    color: ThemeColors.text
                }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true

                    TextArea {
                        id: releaseNotesText
                        readOnly: true
                        text: releaseNotes
                        wrapMode: TextEdit.Wrap
                        textFormat: TextEdit.MarkdownText
                        color: ThemeColors.text

                        background: Rectangle {
                            color: ThemeColors.base
                            border.color: ThemeColors.mid
                            border.width: 1
                            radius: 4
                        }
                    }
                }
            }
        }

        // Button row
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingS

            ThemedButton {
                text: qsTr("Skip This Version")
                flatStyle: true
                onClicked: {
                    skipVersion();
                    updateDialog.close();
                }
            }

            Item {
                Layout.fillWidth: true
            }

            ThemedButton {
                text: qsTr("Remind Me Later")
                outline: true
                onClicked: {
                    remindLater();
                    updateDialog.close();
                }
            }

            ThemedButton {
                text: qsTr("Install Update")
                success: true
                onClicked: {
                    installUpdate();
                    if (downloadUrl) {
                        Qt.openUrlExternally(downloadUrl);
                    }
                    updateDialog.close();
                }
            }
        }
    }

    background: Rectangle {
        color: ThemeColors.window
        border.color: ThemeColors.mid
        border.width: 1
        radius: 8
    }
}
