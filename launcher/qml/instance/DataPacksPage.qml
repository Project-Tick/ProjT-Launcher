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

Rectangle {
    id: dataPacksPage
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
                text: qsTr("Data Packs")
                font.pointSize: 14
                font.bold: true
                color: ThemeColors.text
            }

            Item {
                Layout.fillWidth: true
            }

            ComboBox {
                id: worldSelector
                Layout.preferredWidth: 200
                model: vm ? vm.worldNames : []
                onCurrentIndexChanged: {
                    if (vm)
                        vm.selectWorldForDataPacks(currentIndex);
                }
            }

            Button {
                text: qsTr("Add")
                icon.name: "list-add"
                enabled: worldSelector.currentIndex >= 0
                onClicked: {
                    if (vm)
                        vm.browseForDataPacks();
                }
            }

            Button {
                text: qsTr("Download")
                icon.name: "download"
                enabled: worldSelector.currentIndex >= 0
                onClicked: {
                    if (vm)
                        vm.openDataPackDownload();
                }
            }

            Button {
                text: qsTr("Refresh")
                icon.name: "view-refresh"
                onClicked: {
                    if (vm)
                        vm.refreshDataPacks();
                }
            }
        }

        Label {
            text: qsTr("Data packs are per-world. Select a world to manage its data packs.")
            color: ThemeColors.textSecondary
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            visible: worldSelector.count === 0
        }

        // Data pack list
        Frame {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ListView {
                id: dataPacksList
                anchors.fill: parent
                clip: true
                model: vm ? vm.dataPacksModel : []

                delegate: ItemDelegate {
                    width: dataPacksList.width
                    height: 56

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: Theme.spacingS
                        spacing: Theme.spacingS

                        CheckBox {
                            checked: model.enabled !== false
                            onCheckedChanged: {
                                if (vm)
                                    vm.setDataPackEnabled(index, checked);
                            }
                        }

                        Image {
                            Layout.preferredWidth: 40
                            Layout.preferredHeight: 40
                            source: model.iconPath || ""
                            fillMode: Image.PreserveAspectFit

                            Rectangle {
                                anchors.fill: parent
                                visible: parent.status !== Image.Ready
                                color: ThemeColors.backgroundAlt
                                radius: 4

                                Label {
                                    anchors.centerIn: parent
                                    text: "📊"
                                    font.pointSize: 16
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
                                    vm.deleteDataPack(index);
                            }
                        }
                    }
                }

                ScrollBar.vertical: ScrollBar {}
            }

            Label {
                anchors.centerIn: parent
                visible: dataPacksList.count === 0
                text: worldSelector.currentIndex >= 0 ? qsTr("No data packs in this world.") : qsTr("Select a world to view data packs.")
                color: ThemeColors.textSecondary
                horizontalAlignment: Text.AlignHCenter
            }
        }

        Label {
            text: vm ? qsTr("%1 data packs").arg(vm.dataPacksCount || 0) : ""
            color: ThemeColors.textSecondary
        }
    }
}
