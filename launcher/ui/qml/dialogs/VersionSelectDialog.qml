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
    id: versionSelectDialog
    title: qsTr("Select Version")
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel
    width: 500
    height: 500

    property string selectedVersion: ""
    property var vm: null
    property bool showReleases: true
    property bool showSnapshots: false
    property bool showOldVersions: false

    ColumnLayout {
        anchors.fill: parent
        spacing: Theme.spacingS

        // Filters
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingM

            CheckBox {
                text: qsTr("Releases")
                checked: showReleases
                onCheckedChanged: showReleases = checked
            }

            CheckBox {
                text: qsTr("Snapshots")
                checked: showSnapshots
                onCheckedChanged: showSnapshots = checked
            }

            CheckBox {
                text: qsTr("Old Versions")
                checked: showOldVersions
                onCheckedChanged: showOldVersions = checked
            }

            Item {
                Layout.fillWidth: true
            }

            ThemedButton {
                text: qsTr("Refresh")
                flatStyle: true
                size: "small"
                onClicked: {
                    if (vm)
                        vm.refreshVersions();
                }
            }
        }

        // Search
        TextField {
            id: searchField
            Layout.fillWidth: true
            placeholderText: qsTr("Search versions...")
        }

        // Version list
        Frame {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ListView {
                id: versionsList
                anchors.fill: parent
                clip: true
                model: vm ? vm.filteredVersions : []

                delegate: ItemDelegate {
                    width: versionsList.width
                    highlighted: modelData === selectedVersion

                    visible: {
                        // Filter by search
                        if (searchField.text.length > 0) {
                            if (!modelData.toLowerCase().includes(searchField.text.toLowerCase())) {
                                return false;
                            }
                        }
                        return true;
                    }
                    height: visible ? 36 : 0

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: Theme.spacingS
                        spacing: Theme.spacingS

                        Rectangle {
                            Layout.preferredWidth: 24
                            Layout.preferredHeight: 24
                            radius: 4
                            color: {
                                if (modelData.includes("snapshot") || modelData.includes("pre") || modelData.includes("rc")) {
                                    return ThemeColors.warning; // Snapshot - amber
                                }
                                if (modelData.startsWith("b") || modelData.startsWith("a") || modelData.includes("inf")) {
                                    return "#8b5cf6"; // Old - purple
                                }
                                return ThemeColors.success; // Release - green
                            }

                            Label {
                                anchors.centerIn: parent
                                text: {
                                    if (modelData.includes("snapshot") || modelData.includes("pre") || modelData.includes("rc"))
                                        return "S";
                                    if (modelData.startsWith("b") || modelData.startsWith("a"))
                                        return "O";
                                    return "R";
                                }
                                color: "white"
                                font.bold: true
                                font.pointSize: 10
                            }
                        }

                        Label {
                            text: modelData
                            color: ThemeColors.text
                            Layout.fillWidth: true
                        }
                    }

                    onClicked: {
                        selectedVersion = modelData;
                    }

                    onDoubleClicked: {
                        selectedVersion = modelData;
                        versionSelectDialog.accept();
                    }
                }

                ScrollBar.vertical: ScrollBar {}
            }

            BusyIndicator {
                anchors.centerIn: parent
                running: vm ? vm.loadingVersions : false
                visible: running
            }
        }

        // Selected version
        RowLayout {
            Layout.fillWidth: true

            Label {
                text: qsTr("Selected:")
                color: ThemeColors.textSecondary
            }

            Label {
                text: selectedVersion.length > 0 ? selectedVersion : qsTr("None")
                color: selectedVersion.length > 0 ? ThemeColors.accent : ThemeColors.textSecondary
                font.bold: selectedVersion.length > 0
            }

            Item {
                Layout.fillWidth: true
            }
        }
    }
}
