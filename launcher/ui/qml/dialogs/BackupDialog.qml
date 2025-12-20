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
    id: backupDialog
    title: qsTr("Manage Backups")
    modal: true
    width: 700
    height: 500
    standardButtons: Dialog.Close

    property var vm: ProjT.instancesVM
    property string instanceId: ""
    property string instanceName: ""
    property int selectedBackupIndex: -1

    RowLayout {
        anchors.fill: parent
        spacing: Theme.spacingM

        // Left panel - Backup list
        ColumnLayout {
            Layout.preferredWidth: parent.width * 0.45
            Layout.fillHeight: true
            spacing: Theme.spacingS

            Label {
                text: qsTr("Available Backups:")
                color: ThemeColors.text
                font.bold: true
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: ThemeColors.backgroundAlt
                border.color: ThemeColors.border
                radius: Theme.radiusS

                ListView {
                    id: backupList
                    anchors.fill: parent
                    anchors.margins: 1
                    clip: true
                    model: vm ? vm.backupsList : []
                    currentIndex: selectedBackupIndex

                    delegate: ItemDelegate {
                        width: backupList.width
                        height: 50
                        highlighted: index === backupList.currentIndex

                        onClicked: {
                            backupList.currentIndex = index;
                            selectedBackupIndex = index;
                        }

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: Theme.spacingS
                            spacing: 2

                            Label {
                                text: modelData.name || modelData
                                color: ThemeColors.text
                                font.bold: true
                                elide: Text.ElideRight
                                Layout.fillWidth: true
                            }

                            Label {
                                text: (modelData.date || "") + " - " + (modelData.size || "")
                                color: ThemeColors.textSecondary
                                font.pixelSize: 11
                            }
                        }
                    }

                    ScrollBar.vertical: ScrollBar {}
                }

                Label {
                    anchors.centerIn: parent
                    visible: backupList.count === 0
                    text: qsTr("No backups available")
                    color: ThemeColors.textSecondary
                }
            }

            // Buttons
            RowLayout {
                Layout.fillWidth: true
                spacing: Theme.spacingS

                ThemedButton {
                    text: qsTr("Create")
                    primary: true
                    onClicked: createBackup()
                }

                ThemedButton {
                    text: qsTr("Restore")
                    success: true
                    enabled: selectedBackupIndex >= 0
                    onClicked: {
                        if (vm && selectedBackupIndex >= 0) {
                            vm.restoreBackup(instanceId, vm.backupsList[selectedBackupIndex].path);
                        }
                    }
                }

                ThemedButton {
                    text: qsTr("Delete")
                    danger: true
                    enabled: selectedBackupIndex >= 0
                    onClicked: {
                        if (vm && selectedBackupIndex >= 0) {
                            vm.deleteBackup(vm.backupsList[selectedBackupIndex].path);
                            selectedBackupIndex = -1;
                        }
                    }
                }

                ThemedButton {
                    text: qsTr("Refresh")
                    flatStyle: true
                    onClicked: {
                        if (vm)
                            vm.loadBackupsList(instanceId);
                    }
                }
            }
        }

        // Right panel - Details and options
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: Theme.spacingS

            // Backup Details
            GroupBox {
                Layout.fillWidth: true
                Layout.preferredHeight: 150
                title: qsTr("Backup Details")

                background: Rectangle {
                    color: "transparent"
                    border.color: ThemeColors.border
                    radius: Theme.radiusS
                }

                label: Label {
                    x: Theme.spacingS
                    text: parent.title
                    color: ThemeColors.text
                }

                ScrollView {
                    anchors.fill: parent
                    clip: true

                    TextArea {
                        id: backupDetails
                        readOnly: true
                        wrapMode: TextEdit.Wrap
                        color: ThemeColors.text
                        text: selectedBackupIndex >= 0 && vm && vm.backupsList[selectedBackupIndex] ? qsTr("Name: %1\nDate: %2\nSize: %3\nPath: %4").arg(vm.backupsList[selectedBackupIndex].name || "", vm.backupsList[selectedBackupIndex].date || "", vm.backupsList[selectedBackupIndex].size || "", vm.backupsList[selectedBackupIndex].path || "") : qsTr("Select a backup to view details")

                        background: Rectangle {
                            color: ThemeColors.backgroundAlt
                        }
                    }
                }
            }

            // Backup Options
            GroupBox {
                Layout.fillWidth: true
                Layout.fillHeight: true
                title: qsTr("Backup Options")

                background: Rectangle {
                    implicitWidth: 0
                    implicitHeight: 0
                    y: parent.topPadding - parent.padding
                    width: parent.width
                    height: parent.height - parent.topPadding + parent.padding
                    color: "transparent"
                    border.color: ThemeColors.border
                    radius: Theme.radiusS
                }

                label: Label {
                    x: Theme.spacingS
                    text: parent.title
                    color: ThemeColors.text
                }

                ColumnLayout {
                    anchors.fill: parent
                    spacing: Theme.spacingXS

                    CheckBox {
                        id: includeSaves
                        text: qsTr("Include Saves")
                        checked: true
                    }

                    CheckBox {
                        id: includeConfig
                        text: qsTr("Include Config")
                        checked: true
                    }

                    CheckBox {
                        id: includeMods
                        text: qsTr("Include Mods")
                        checked: false
                    }

                    CheckBox {
                        id: includeResourcePacks
                        text: qsTr("Include Resource Packs")
                        checked: false
                    }

                    CheckBox {
                        id: includeShaderPacks
                        text: qsTr("Include Shader Packs")
                        checked: false
                    }

                    CheckBox {
                        id: includeScreenshots
                        text: qsTr("Include Screenshots")
                        checked: false
                    }

                    CheckBox {
                        id: includeOptions
                        text: qsTr("Include Options (options.txt)")
                        checked: true
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 1
                        color: ThemeColors.border
                    }

                    Label {
                        text: qsTr("Custom Paths:")
                        color: ThemeColors.text
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 80
                        color: ThemeColors.backgroundAlt
                        border.color: ThemeColors.border
                        radius: Theme.radiusS

                        ListView {
                            id: customPathsList
                            anchors.fill: parent
                            anchors.margins: 1
                            clip: true
                            model: []

                            delegate: ItemDelegate {
                                width: customPathsList.width
                                text: modelData
                            }
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: Theme.spacingS

                        ThemedButton {
                            text: qsTr("Add...")
                            primary: true
                            size: "small"
                            onClicked: {
                                if (ProjT && ProjT.launcherVM) {
                                    var path = ProjT.launcherVM.browseForDirectory(qsTr("Select custom backup path"));
                                    if (path && path.length > 0) {
                                        var paths = customPathsList.model.slice();
                                        paths.push(path);
                                        customPathsList.model = paths;
                                    }
                                }
                            }
                        }

                        ThemedButton {
                            text: qsTr("Remove")
                            danger: true
                            size: "small"
                            enabled: customPathsList.currentIndex >= 0
                            onClicked: {
                                var paths = customPathsList.model.slice();
                                paths.splice(customPathsList.currentIndex, 1);
                                customPathsList.model = paths;
                            }
                        }
                    }
                }
            }
        }
    }

    function createBackup() {
        if (!vm)
            return;
        var options = {
            includeSaves: includeSaves.checked,
            includeConfig: includeConfig.checked,
            includeMods: includeMods.checked,
            includeResourcePacks: includeResourcePacks.checked,
            includeShaderPacks: includeShaderPacks.checked,
            includeScreenshots: includeScreenshots.checked,
            includeOptions: includeOptions.checked,
            customPaths: customPathsList.model
        };

        vm.createBackup(instanceId, instanceName + "_backup_" + Qt.formatDateTime(new Date(), "yyyyMMdd_HHmmss"), options);
    }

    onOpened: {
        if (vm)
            vm.loadBackupsList(instanceId);
        selectedBackupIndex = -1;
    }

    Connections {
        target: vm
        ignoreUnknownSignals: true
        function onCustomPathSelected(path) {
            var paths = customPathsList.model.slice();
            paths.push(path);
            customPathsList.model = paths;
        }
    }
}
