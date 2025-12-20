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

Dialog {
    id: installLoaderDialog
    title: qsTr("Install Mod Loader")
    modal: true
    standardButtons: Dialog.Cancel
    width: 500
    height: 450

    property var vm: null
    property string minecraftVersion: ""
    property string selectedLoader: ""
    property string selectedLoaderVersion: ""

    ColumnLayout {
        anchors.fill: parent
        spacing: Theme.spacingM

        // Loader selection
        Label {
            text: qsTr("Select a mod loader to install:")
            color: ThemeColors.text
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingS

            Repeater {
                model: [
                    {
                        id: "forge",
                        name: "Forge",
                        color: "#dfa86a"
                    },
                    {
                        id: "neoforge",
                        name: "NeoForge",
                        color: "#d64541"
                    },
                    {
                        id: "fabric",
                        name: "Fabric",
                        color: "#dbd0b4"
                    },
                    {
                        id: "quilt",
                        name: "Quilt",
                        color: "#9b59b6"
                    },
                    {
                        id: "liteloader",
                        name: "LiteLoader",
                        color: "#3498db"
                    }
                ]

                delegate: Button {
                    Layout.fillWidth: true
                    text: modelData.name
                    highlighted: selectedLoader === modelData.id

                    background: Rectangle {
                        implicitHeight: 50
                        radius: 8
                        color: highlighted ? modelData.color : ThemeColors.backgroundAlt
                        border.color: highlighted ? modelData.color : ThemeColors.border
                        border.width: 1
                    }

                    contentItem: Label {
                        text: modelData.name
                        color: highlighted ? (modelData.id === "fabric" ? "#333" : "white") : ThemeColors.text
                        font.bold: highlighted
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    onClicked: {
                        selectedLoader = modelData.id;
                        selectedLoaderVersion = "";
                        if (vm)
                            vm.fetchLoaderVersions(modelData.id, minecraftVersion);
                    }
                }
            }
        }

        // Version selection
        GroupBox {
            Layout.fillWidth: true
            Layout.fillHeight: true
            title: qsTr("Version")
            enabled: selectedLoader.length > 0

            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS

                CheckBox {
                    id: showAllVersions
                    text: qsTr("Show all versions (including unstable)")
                    checked: false
                }

                Frame {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    ListView {
                        id: versionsList
                        anchors.fill: parent
                        clip: true
                        model: vm ? vm.loaderVersions : []

                        delegate: ItemDelegate {
                            width: versionsList.width
                            height: visible ? 44 : 0
                            visible: showAllVersions.checked || modelData.stable === true
                            highlighted: modelData.version === selectedLoaderVersion

                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: Theme.spacingS
                                spacing: Theme.spacingM

                                Label {
                                    text: modelData.version || modelData
                                    color: ThemeColors.text
                                    Layout.fillWidth: true
                                }

                                // Recommended badge
                                Rectangle {
                                    Layout.preferredWidth: recLabel.implicitWidth + 12
                                    Layout.preferredHeight: 20
                                    radius: 10
                                    color: ThemeColors.success
                                    visible: modelData.recommended === true

                                    Label {
                                        id: recLabel
                                        anchors.centerIn: parent
                                        text: qsTr("Recommended")
                                        color: "white"
                                        font.pointSize: Theme.fontSizeSmall - 1
                                    }
                                }

                                // Latest badge
                                Rectangle {
                                    Layout.preferredWidth: latestLabel.implicitWidth + 12
                                    Layout.preferredHeight: 20
                                    radius: 10
                                    color: ThemeColors.accent
                                    visible: modelData.latest === true && modelData.recommended !== true

                                    Label {
                                        id: latestLabel
                                        anchors.centerIn: parent
                                        text: qsTr("Latest")
                                        color: "white"
                                        font.pointSize: Theme.fontSizeSmall - 1
                                    }
                                }

                                // Unstable indicator
                                Label {
                                    text: qsTr("(unstable)")
                                    color: ThemeColors.warning
                                    font.pointSize: Theme.fontSizeSmall
                                    visible: modelData.stable === false
                                }
                            }

                            onClicked: {
                                selectedLoaderVersion = modelData.version || modelData;
                            }

                            onDoubleClicked: {
                                selectedLoaderVersion = modelData.version || modelData;
                                installLoader();
                            }
                        }

                        ScrollBar.vertical: ScrollBar {}
                    }

                    BusyIndicator {
                        anchors.centerIn: parent
                        running: vm ? vm.loadingLoaderVersions : false
                        visible: running
                    }

                    Label {
                        anchors.centerIn: parent
                        text: selectedLoader.length > 0 ? qsTr("No versions available") : qsTr("Select a loader first")
                        color: ThemeColors.textSecondary
                        visible: !vm || (!vm.loadingLoaderVersions && vm.loaderVersions.length === 0)
                    }
                }
            }
        }

        // Info
        Label {
            Layout.fillWidth: true
            text: qsTr("For Minecraft %1").arg(minecraftVersion)
            color: ThemeColors.textSecondary
            font.pointSize: Theme.fontSizeSmall
            visible: minecraftVersion.length > 0
        }

        // Install button
        Button {
            Layout.fillWidth: true
            text: qsTr("Install %1 %2").arg(selectedLoader.charAt(0).toUpperCase() + selectedLoader.slice(1)).arg(selectedLoaderVersion)
            highlighted: true
            enabled: selectedLoader.length > 0 && selectedLoaderVersion.length > 0
            onClicked: installLoader()
        }
    }

    function installLoader() {
        if (vm && selectedLoader.length > 0 && selectedLoaderVersion.length > 0) {
            vm.installLoader(selectedLoader, selectedLoaderVersion);
            installLoaderDialog.accept();
        }
    }
}
