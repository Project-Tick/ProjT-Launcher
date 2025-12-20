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
    id: exportDialog
    title: qsTr("Export Instance")
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel
    width: 600
    height: 500

    property var vm: ProjT.instancesVM
    property string instanceId: ""
    property string instanceName: ""

    ColumnLayout {
        anchors.fill: parent
        spacing: Theme.spacingM

        Label {
            text: qsTr("Export '%1' as:").arg(instanceName)
            font.bold: true
            color: ThemeColors.text
        }

        // Export format
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Export Format")

            ColumnLayout {
                anchors.fill: parent

                RadioButton {
                    id: formatProjT
                    text: qsTr("ProjT Launcher (.zip)")
                    checked: true
                }

                RadioButton {
                    id: formatCurseForge
                    text: qsTr("CurseForge (.zip)")
                }

                RadioButton {
                    id: formatModrinth
                    text: qsTr("Modrinth (.mrpack)")
                }

                RadioButton {
                    id: formatModList
                    text: qsTr("Mod List (.txt)")
                }
            }
        }

        // File selection
        GroupBox {
            Layout.fillWidth: true
            Layout.fillHeight: true
            title: qsTr("Include Files")
            visible: !formatModList.checked

            ColumnLayout {
                anchors.fill: parent

                RowLayout {
                    Layout.fillWidth: true

                    Button {
                        text: qsTr("Select All")
                        onClicked: {
                            for (var i = 0; i < filesList.count; i++) {
                                filesList.model[i].checked = true;
                            }
                            filesList.model = filesList.model;
                        }
                    }

                    Button {
                        text: qsTr("Select None")
                        onClicked: {
                            for (var i = 0; i < filesList.count; i++) {
                                filesList.model[i].checked = false;
                            }
                            filesList.model = filesList.model;
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                    }
                }

                ListView {
                    id: filesList
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    model: vm ? vm.exportFileList : []

                    delegate: CheckDelegate {
                        width: filesList.width
                        text: modelData.name || modelData
                        checked: modelData.checked !== false
                        onCheckedChanged: {
                            modelData.checked = checked;
                        }
                    }

                    ScrollBar.vertical: ScrollBar {}
                }
            }
        }

        // Output location
        RowLayout {
            Layout.fillWidth: true

            Label {
                text: qsTr("Save to:")
                color: ThemeColors.text
            }

            TextField {
                id: outputPath
                Layout.fillWidth: true
                placeholderText: qsTr("Select output location...")
                readOnly: true
            }

            Button {
                text: qsTr("Browse...")
                onClicked: {
                    if (ProjT && ProjT.launcherVM) {
                        var filter = formatModList.checked ? qsTr("Text files (*.txt)") : qsTr("Zip files (*.zip)");
                        var path = ProjT.launcherVM.browseForSave(qsTr("Select export location"), filter);
                        if (path && path.length > 0) {
                            outputPath.text = path;
                        }
                    }
                }
            }
        }
    }

    onAccepted: {
        if (vm && outputPath.text.length > 0) {
            var format = "projt";
            if (formatCurseForge.checked)
                format = "curseforge";
            else if (formatModrinth.checked)
                format = "modrinth";
            else if (formatModList.checked)
                format = "modlist";

            vm.exportInstanceSimple(instanceId, outputPath.text, format);
        }
    }
}
