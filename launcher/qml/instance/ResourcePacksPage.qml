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

Rectangle {
    id: resourcePacksPage
    color: ThemeColors.background

    property var vm: ProjT.instanceVM

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingM
        spacing: Theme.spacingS

        // Header
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingS

            Label {
                text: qsTr("Resource Packs")
                font.pointSize: 14
                font.bold: true
                color: ThemeColors.text
            }

            Item {
                Layout.fillWidth: true
            }

            Button {
                text: qsTr("Add")
                icon.name: "list-add"
                onClicked: {
                    if (vm)
                        vm.browseForResourcePacks();
                }
            }

            Button {
                text: qsTr("Download")
                icon.name: "download"
                onClicked: {
                    if (vm)
                        vm.openResourcePackDownload();
                }
            }

            Button {
                text: qsTr("Refresh")
                icon.name: "view-refresh"
                onClicked: {
                    if (vm)
                        vm.refreshResourcePacks();
                }
            }
        }

        // Resource pack list
        Frame {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ListView {
                id: packsList
                anchors.fill: parent
                clip: true
                model: vm ? vm.resourcePacksModel : []

                delegate: ItemDelegate {
                    width: packsList.width
                    height: 64

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: Theme.spacingS
                        spacing: Theme.spacingS

                        Image {
                            Layout.preferredWidth: 48
                            Layout.preferredHeight: 48
                            source: model.iconPath || ""
                            fillMode: Image.PreserveAspectFit

                            Rectangle {
                                anchors.fill: parent
                                visible: parent.status !== Image.Ready
                                color: ThemeColors.backgroundAlt
                                radius: 4

                                Label {
                                    anchors.centerIn: parent
                                    text: "🎨"
                                    font.pointSize: 20
                                }
                            }
                        }

                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2

                            Label {
                                text: model.name || model.fileName || ""
                                color: ThemeColors.text
                                font.bold: true
                                elide: Text.ElideRight
                                Layout.fillWidth: true
                            }

                            Label {
                                text: model.description || ""
                                color: ThemeColors.textSecondary
                                font.pointSize: 9
                                elide: Text.ElideRight
                                Layout.fillWidth: true
                            }
                        }

                        ToolButton {
                            icon.name: "edit-delete"
                            onClicked: {
                                if (vm)
                                    vm.removeResourcePack(index);
                            }
                        }
                    }
                }

                ScrollBar.vertical: ScrollBar {}
            }

            Label {
                anchors.centerIn: parent
                visible: packsList.count === 0
                text: qsTr("No resource packs installed.\nClick 'Add' or 'Download' to add resource packs.")
                color: ThemeColors.textSecondary
                horizontalAlignment: Text.AlignHCenter
            }
        }

        // Info
        Label {
            text: vm ? qsTr("%1 resource packs").arg(vm.resourcePacksCount || 0) : ""
            color: ThemeColors.textSecondary
        }
    }
}
