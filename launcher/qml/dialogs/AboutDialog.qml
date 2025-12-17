// SPDX-License-Identifier: GPL-3.0-or-later
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

Dialog {
    id: aboutDialog
    title: qsTr("About ProjT Launcher")
    modal: true
    width: 573
    height: 600
    standardButtons: Dialog.NoButton

    property var appInfo: ProjT ? ProjT.appInfo : null

    ColumnLayout {
        anchors.fill: parent
        spacing: Theme.spacingS

        // Logo centered
        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: 80

            Image {
                anchors.centerIn: parent
                width: 64
                height: 64
                source: "qrc:/launcher/icon.png"
                fillMode: Image.PreserveAspectFit
            }
        }

        // Title
        Label {
            Layout.fillWidth: true
            text: "ProjT Launcher"
            color: ThemeColors.text
            font.pointSize: 15
            horizontalAlignment: Text.AlignHCenter
        }

        // Version (selectable)
        Label {
            Layout.alignment: Qt.AlignHCenter
            text: qsTr("Version %1").arg(appInfo ? appInfo.version : "")
            color: ThemeColors.textSecondary

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.IBeamCursor
            }
        }

        // TabWidget
        TabBar {
            id: tabBar
            Layout.fillWidth: true

            TabButton {
                text: qsTr("About")
            }
            TabButton {
                text: qsTr("Credits")
            }
            TabButton {
                text: qsTr("License")
            }
        }

        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: tabBar.currentIndex

            // About Tab
            ColumnLayout {
                spacing: Theme.spacingS

                Label {
                    Layout.fillWidth: true
                    text: qsTr("A custom launcher that makes managing Minecraft easier by allowing you to have multiple instances of Minecraft at once.")
                    color: ThemeColors.textSecondary
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter
                }

                Label {
                    Layout.fillWidth: true
                    text: "<a href='https://github.com/Project-Tick/ProjT-Launcher'>https://github.com/Project-Tick/ProjT-Launcher</a>"
                    color: ThemeColors.accent
                    font.pointSize: 10
                    horizontalAlignment: Text.AlignHCenter
                    onLinkActivated: link => Qt.openUrlExternally(link)

                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.NoButton
                        cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                    }
                }

                Label {
                    Layout.fillWidth: true
                    text: appInfo ? appInfo.copyright : "© 2025 Project Tick Contributors"
                    color: ThemeColors.textSecondary
                    font.pointSize: 8
                    horizontalAlignment: Text.AlignHCenter
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 1
                    color: ThemeColors.border
                }

                // Platform info
                Label {
                    Layout.alignment: Qt.AlignHCenter
                    text: qsTr("Platform: %1").arg(appInfo ? appInfo.platform : Qt.platform.os)
                    color: ThemeColors.textSecondary

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.IBeamCursor
                    }
                }

                // Build date
                Label {
                    Layout.alignment: Qt.AlignHCenter
                    text: qsTr("Build Date: %1").arg(appInfo ? appInfo.buildDate : "")
                    color: ThemeColors.textSecondary

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.IBeamCursor
                    }
                }

                // Commit
                Label {
                    Layout.alignment: Qt.AlignHCenter
                    text: qsTr("Commit: %1").arg(appInfo ? appInfo.gitCommit : "")
                    color: ThemeColors.textSecondary

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.IBeamCursor
                    }
                }

                // Channel
                Label {
                    Layout.alignment: Qt.AlignHCenter
                    text: qsTr("Channel: %1").arg(appInfo ? appInfo.channel : "stable")
                    color: ThemeColors.textSecondary

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.IBeamCursor
                    }
                }

                Item {
                    Layout.fillHeight: true
                }
            }

            // Credits Tab
            ScrollView {
                clip: true

                TextArea {
                    id: creditsText
                    readOnly: true
                    textFormat: TextArea.RichText
                    text: appInfo ? appInfo.credits : qsTr("<h3>Contributors</h3><p>Thanks to all the amazing contributors who have helped make this project possible!</p><p>Based on PolyMC and Prism Launcher.</p>")
                    wrapMode: Text.WordWrap
                    color: ThemeColors.text
                    onLinkActivated: link => Qt.openUrlExternally(link)
                    background: Rectangle {
                        color: "transparent"
                    }
                }
            }

            // License Tab
            ScrollView {
                clip: true

                TextArea {
                    id: licenseText
                    readOnly: true
                    font.family: "DejaVu Sans Mono"
                    text: qsTr("ProjT Launcher is licensed under the GNU General Public License v3.0.\n\nThis program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.\n\nThis program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.\n\nYou should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.")
                    wrapMode: Text.WordWrap
                    color: ThemeColors.text
                    background: Rectangle {
                        color: "transparent"
                    }
                }
            }
        }

        // Bottom buttons
        RowLayout {
            Layout.fillWidth: true

            Button {
                text: qsTr("About Qt")
                onClicked: {
                    if (ProjT && ProjT.showAboutQt) {
                        ProjT.showAboutQt();
                    }
                }
            }

            Item {
                Layout.fillWidth: true
            }

            Button {
                text: qsTr("Close")
                onClicked: aboutDialog.close()
            }
        }
    }
}
