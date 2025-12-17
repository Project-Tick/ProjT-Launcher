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
    id: modsPage
    color: ThemeColors.background

    property var vm: ProjT.instanceVM
    property var selectedIndices: []

    RowLayout {
        anchors.fill: parent
        spacing: 0

        // Main content
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            // Mods TreeView
            Frame {
                Layout.fillWidth: true
                Layout.fillHeight: true

                ListView {
                    id: modsList
                    anchors.fill: parent
                    clip: true
                    model: vm ? vm.modsModel : []

                    delegate: ItemDelegate {
                        width: modsList.width
                        height: 48
                        highlighted: selectedIndices.indexOf(index) >= 0

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: Theme.spacingS
                            spacing: Theme.spacingS

                            CheckBox {
                                checked: model.enabled !== false
                                onCheckedChanged: {
                                    if (vm)
                                        vm.enableMod(index, checked);
                                }
                            }

                            Image {
                                Layout.preferredWidth: 32
                                Layout.preferredHeight: 32
                                source: model.iconPath || ""
                                fillMode: Image.PreserveAspectFit

                                Rectangle {
                                    anchors.fill: parent
                                    visible: parent.status !== Image.Ready
                                    color: ThemeColors.backgroundAlt
                                    radius: 4

                                    Label {
                                        anchors.centerIn: parent
                                        text: "📦"
                                        font.pointSize: 12
                                    }
                                }
                            }

                            Label {
                                text: model.name || model.fileName || ""
                                color: model.enabled !== false ? ThemeColors.text : ThemeColors.textSecondary
                                Layout.fillWidth: true
                                elide: Text.ElideRight
                            }

                            Label {
                                text: model.version || ""
                                color: ThemeColors.textSecondary
                            }
                        }

                        onClicked: {
                            if (mouse.modifiers & Qt.ControlModifier) {
                                var idx = selectedIndices.indexOf(index);
                                if (idx >= 0) {
                                    selectedIndices.splice(idx, 1);
                                } else {
                                    selectedIndices.push(index);
                                }
                                selectedIndices = selectedIndices.slice();
                            } else {
                                selectedIndices = [index];
                            }
                        }
                    }

                    ScrollBar.vertical: ScrollBar {}

                    Label {
                        anchors.centerIn: parent
                        visible: modsList.count === 0
                        text: qsTr("No mods installed.\nClick 'Add File' or 'Download' to add mods.")
                        color: ThemeColors.textSecondary
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }

            // Info frame
            Frame {
                Layout.fillWidth: true
                Layout.preferredHeight: 60
                visible: selectedIndices.length === 1

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 4

                    Label {
                        text: {
                            if (selectedIndices.length === 1 && vm && vm.modsModel) {
                                return vm.modsModel[selectedIndices[0]]?.name || "";
                            }
                            return "";
                        }
                        font.bold: true
                        color: ThemeColors.text
                    }

                    Label {
                        Layout.fillWidth: true
                        text: {
                            if (selectedIndices.length === 1 && vm && vm.modsModel) {
                                return vm.modsModel[selectedIndices[0]]?.description || "";
                            }
                            return "";
                        }
                        color: ThemeColors.textSecondary
                        wrapMode: Text.WordWrap
                        elide: Text.ElideRight
                        maximumLineCount: 2
                    }
                }
            }

            // Search filter
            TextField {
                id: filterEdit
                Layout.fillWidth: true
                placeholderText: qsTr("Search")
                onTextChanged: {
                    if (vm)
                        vm.filterMods(text);
                }
            }
        }

        // Right toolbar
        Rectangle {
            Layout.preferredWidth: 140
            Layout.fillHeight: true
            color: ThemeColors.backgroundAlt

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: Theme.spacingS
                spacing: 2

                Label {
                    text: qsTr("Actions")
                    font.bold: true
                    color: ThemeColors.textSecondary
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                }

                ToolButton {
                    text: qsTr("&Add File")
                    Layout.fillWidth: true
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Add a locally downloaded file.")
                    onClicked: {
                        if (vm)
                            vm.browseForMods();
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 1
                    color: ThemeColors.border
                }

                ToolButton {
                    text: qsTr("&Remove")
                    Layout.fillWidth: true
                    enabled: selectedIndices.length > 0
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Remove all selected items.")
                    onClicked: {
                        deleteModDialog.open();
                    }
                }

                ToolButton {
                    text: qsTr("&Enable")
                    Layout.fillWidth: true
                    enabled: selectedIndices.length > 0
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Enable all selected items.")
                    onClicked: {
                        if (vm) {
                            for (var i = 0; i < selectedIndices.length; i++) {
                                vm.enableMod(selectedIndices[i], true);
                            }
                        }
                    }
                }

                ToolButton {
                    text: qsTr("&Disable")
                    Layout.fillWidth: true
                    enabled: selectedIndices.length > 0
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Disable all selected items.")
                    onClicked: {
                        if (vm) {
                            for (var i = 0; i < selectedIndices.length; i++) {
                                vm.enableMod(selectedIndices[i], false);
                            }
                        }
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 1
                    color: ThemeColors.border
                }

                ToolButton {
                    text: qsTr("&Download")
                    Layout.fillWidth: true
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Download resources from online mod platforms.")
                    onClicked: {
                        if (vm)
                            vm.openModDownload();
                    }
                }

                ToolButton {
                    text: qsTr("Check for &Updates")
                    Layout.fillWidth: true
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Try to check or update all selected resources.")
                    onClicked: {
                        if (vm)
                            vm.checkAllModUpdates();
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 1
                    color: ThemeColors.border
                }

                ToolButton {
                    text: qsTr("View &Configs")
                    Layout.fillWidth: true
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Open the 'config' folder in the system file manager.")
                    onClicked: {
                        if (vm)
                            vm.openConfigsFolder();
                    }
                }

                ToolButton {
                    text: qsTr("View &Folder")
                    Layout.fillWidth: true
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Open the folder in the system file manager.")
                    onClicked: {
                        if (vm)
                            vm.openModsFolder();
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 1
                    color: ThemeColors.border
                }

                ToolButton {
                    text: qsTr("Reset Metadata")
                    Layout.fillWidth: true
                    enabled: selectedIndices.length > 0
                    onClicked: {
                        if (vm)
                            vm.resetModMetadata(selectedIndices);
                    }
                }

                ToolButton {
                    text: qsTr("Verify Dependencies")
                    Layout.fillWidth: true
                    onClicked: {
                        if (vm)
                            vm.verifyDependencies();
                    }
                }

                ToolButton {
                    text: qsTr("Export List")
                    Layout.fillWidth: true
                    enabled: modsList.count > 0
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Export resource's metadata to text.")
                    onClicked: {
                        if (vm)
                            vm.exportModList();
                    }
                }

                Item {
                    Layout.fillHeight: true
                }
            }
        }
    }

    // Delete confirmation
    Dialog {
        id: deleteModDialog
        title: qsTr("Delete Mods")
        modal: true
        standardButtons: Dialog.Yes | Dialog.No
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2

        Label {
            text: qsTr("Delete %1 selected mod(s)?\n\nThis cannot be undone.").arg(selectedIndices.length)
            color: ThemeColors.error
            wrapMode: Text.WordWrap
        }

        onAccepted: {
            if (vm) {
                for (var i = selectedIndices.length - 1; i >= 0; i--) {
                    vm.removeMod(selectedIndices[i]);
                }
                selectedIndices = [];
            }
        }
    }
}
