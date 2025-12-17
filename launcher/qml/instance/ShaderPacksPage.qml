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
    id: shaderPacksPage
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
                text: qsTr("Shader Packs")
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
                        vm.browseForShaderPacks();
                }
            }

            Button {
                text: qsTr("Download")
                icon.name: "download"
                onClicked: {
                    if (vm)
                        vm.openShaderPackDownload();
                }
            }

            Button {
                text: qsTr("Refresh")
                icon.name: "view-refresh"
                onClicked: {
                    if (vm)
                        vm.refreshShaderPacks();
                }
            }
        }

        // Shader pack list
        Frame {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ListView {
                id: shadersList
                anchors.fill: parent
                clip: true
                model: vm ? vm.shaderPacksModel : []

                delegate: ItemDelegate {
                    width: shadersList.width
                    height: 56

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: Theme.spacingS
                        spacing: Theme.spacingS

                        Rectangle {
                            Layout.preferredWidth: 40
                            Layout.preferredHeight: 40
                            color: ThemeColors.backgroundAlt
                            radius: 4

                            Label {
                                anchors.centerIn: parent
                                text: "✨"
                                font.pointSize: 16
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
                                text: model.fileSize || ""
                                color: ThemeColors.textSecondary
                                font.pointSize: 9
                            }
                        }

                        ToolButton {
                            icon.name: "edit-delete"
                            onClicked: {
                                if (vm)
                                    vm.removeShaderPack(index);
                            }
                        }
                    }
                }

                ScrollBar.vertical: ScrollBar {}
            }

            Label {
                anchors.centerIn: parent
                visible: shadersList.count === 0
                text: qsTr("No shader packs installed.\nShader packs require OptiFine or Iris.")
                color: ThemeColors.textSecondary
                horizontalAlignment: Text.AlignHCenter
            }
        }

        Label {
            text: vm ? qsTr("%1 shader packs").arg(vm.shaderPacksCount || 0) : ""
            color: ThemeColors.textSecondary
        }
    }
}
