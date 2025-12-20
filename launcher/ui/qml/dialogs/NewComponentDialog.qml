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

Dialog {
    id: newComponentDialog
    title: qsTr("Add Component")
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel
    width: 450
    height: 350

    property var vm: null
    property string selectedType: ""
    property string selectedVersion: ""

    ColumnLayout {
        anchors.fill: parent
        spacing: Theme.spacingM

        Label {
            text: qsTr("Select a component type to add:")
            color: ThemeColors.text
        }

        // Component type grid
        GridLayout {
            Layout.fillWidth: true
            columns: 2
            columnSpacing: Theme.spacingS
            rowSpacing: Theme.spacingS

            Repeater {
                model: [
                    {
                        id: "minecraft",
                        name: "Minecraft",
                        icon: "minecraft",
                        color: "#4aae46"
                    },
                    {
                        id: "forge",
                        name: "Forge",
                        icon: "forge",
                        color: "#dfa86a"
                    },
                    {
                        id: "neoforge",
                        name: "NeoForge",
                        icon: "neoforge",
                        color: "#d64541"
                    },
                    {
                        id: "fabric",
                        name: "Fabric Loader",
                        icon: "fabric",
                        color: "#dbd0b4"
                    },
                    {
                        id: "quilt",
                        name: "Quilt Loader",
                        icon: "quilt",
                        color: "#9b59b6"
                    },
                    {
                        id: "liteloader",
                        name: "LiteLoader",
                        icon: "liteloader",
                        color: "#3498db"
                    }
                ]

                delegate: Button {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 50
                    highlighted: selectedType === modelData.id

                    background: Rectangle {
                        radius: 8
                        color: highlighted ? modelData.color : ThemeColors.backgroundAlt
                        border.color: highlighted ? modelData.color : ThemeColors.border
                        border.width: 1
                    }

                    contentItem: RowLayout {
                        spacing: Theme.spacingS

                        Rectangle {
                            Layout.preferredWidth: 24
                            Layout.preferredHeight: 24
                            radius: 4
                            color: highlighted ? "white" : modelData.color
                            opacity: highlighted ? 0.3 : 0.2

                            Label {
                                anchors.centerIn: parent
                                text: modelData.name.charAt(0)
                                color: highlighted ? "white" : modelData.color
                                font.bold: true
                            }
                        }

                        Label {
                            text: modelData.name
                            color: highlighted ? (modelData.id === "fabric" ? "#333" : "white") : ThemeColors.text
                            font.bold: highlighted
                        }
                    }

                    onClicked: {
                        selectedType = modelData.id;
                        selectedVersion = "";
                        if (vm)
                            vm.fetchComponentVersions(modelData.id);
                    }
                }
            }
        }

        // Version selection
        GroupBox {
            Layout.fillWidth: true
            Layout.fillHeight: true
            title: qsTr("Version")
            enabled: selectedType.length > 0

            Frame {
                anchors.fill: parent

                ListView {
                    id: versionsList
                    anchors.fill: parent
                    clip: true
                    model: vm ? vm.componentVersions : []

                    delegate: ItemDelegate {
                        width: versionsList.width
                        height: 36
                        highlighted: modelData === selectedVersion

                        Label {
                            anchors.fill: parent
                            anchors.margins: Theme.spacingS
                            text: modelData
                            color: ThemeColors.text
                            verticalAlignment: Text.AlignVCenter
                        }

                        onClicked: selectedVersion = modelData
                        onDoubleClicked: {
                            selectedVersion = modelData;
                            newComponentDialog.accept();
                        }
                    }

                    ScrollBar.vertical: ScrollBar {}
                }

                BusyIndicator {
                    anchors.centerIn: parent
                    running: vm ? vm.loadingComponentVersions : false
                    visible: running
                }

                Label {
                    anchors.centerIn: parent
                    text: selectedType.length > 0 ? qsTr("No versions available") : qsTr("Select a component type")
                    color: ThemeColors.textSecondary
                    visible: !vm || (!vm.loadingComponentVersions && vm.componentVersions.length === 0)
                }
            }
        }

        // Selected info
        Label {
            Layout.fillWidth: true
            text: selectedType.length > 0 && selectedVersion.length > 0 ? qsTr("Will add: %1 %2").arg(selectedType.charAt(0).toUpperCase() + selectedType.slice(1)).arg(selectedVersion) : qsTr("Select a component and version")
            color: selectedType.length > 0 && selectedVersion.length > 0 ? ThemeColors.accent : ThemeColors.textSecondary
            font.italic: selectedType.length === 0 || selectedVersion.length === 0
        }
    }

    onAccepted: {
        if (vm && selectedType.length > 0 && selectedVersion.length > 0) {
            vm.addComponent(selectedType, selectedVersion);
        }
    }
}
