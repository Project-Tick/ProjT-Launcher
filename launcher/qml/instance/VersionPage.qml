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
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import ProjTLauncher 1.0
import "../Theme.js" as Theme

Rectangle {
    id: versionPage
    color: ThemeColors.background

    property var vm: ProjT.instanceVM
    property int selectedIndex: -1

    RowLayout {
        anchors.fill: parent
        spacing: 0

        // Main content
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            // Components ListView
            Frame {
                Layout.fillWidth: true
                Layout.fillHeight: true

                ListView {
                    id: packageView
                    anchors.fill: parent
                    clip: true
                    model: vm ? vm.componentsModel : null

                    delegate: ItemDelegate {
                        width: packageView.width
                        height: 40
                        highlighted: index === selectedIndex

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: Theme.spacingS
                            spacing: Theme.spacingS

                            CheckBox {
                                visible: model.canToggle || false
                                checked: model.enabled !== false
                                onCheckedChanged: {
                                    if (vm)
                                        vm.setComponentEnabled(index, checked);
                                }
                            }

                            Label {
                                text: model.name || ""
                                color: ThemeColors.text
                                Layout.fillWidth: true
                            }

                            Label {
                                text: model.version || ""
                                color: ThemeColors.textSecondary
                            }
                        }

                        onClicked: selectedIndex = index
                    }

                    ScrollBar.vertical: ScrollBar {}
                }
            }

            // Search filter
            TextField {
                id: filterEdit
                Layout.fillWidth: true
                placeholderText: qsTr("Search")
                onTextChanged: {
                    if (vm)
                        vm.filterComponents(text);
                }
            }

            // Info frame
            Frame {
                Layout.fillWidth: true
                Layout.preferredHeight: 80
                visible: selectedIndex >= 0

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 4

                    Label {
                        text: selectedIndex >= 0 && vm && vm.componentsModel ? (vm.componentsModel[selectedIndex]?.name || "") : ""
                        font.bold: true
                        color: ThemeColors.text
                    }

                    Label {
                        Layout.fillWidth: true
                        text: selectedIndex >= 0 && vm && vm.componentsModel ? (vm.componentsModel[selectedIndex]?.description || "") : ""
                        color: ThemeColors.textSecondary
                        wrapMode: Text.WordWrap
                    }
                }
            }
        }

        // Right toolbar
        Rectangle {
            Layout.preferredWidth: 160
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
                    text: qsTr("Change Version")
                    Layout.fillWidth: true
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Change version of the selected component.")
                    onClicked: changeVersionDialog.open()
                }

                ToolButton {
                    text: qsTr("Move Up")
                    Layout.fillWidth: true
                    enabled: selectedIndex > 0
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Make the selected component apply sooner.")
                    onClicked: {
                        if (vm)
                            vm.moveComponentUp(selectedIndex);
                    }
                }

                ToolButton {
                    text: qsTr("Move Down")
                    Layout.fillWidth: true
                    enabled: selectedIndex >= 0
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Make the selected component apply later.")
                    onClicked: {
                        if (vm)
                            vm.moveComponentDown(selectedIndex);
                    }
                }

                ToolButton {
                    text: qsTr("Remove")
                    Layout.fillWidth: true
                    enabled: selectedIndex >= 0
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Remove selected component from the instance.")
                    onClicked: {
                        if (vm)
                            vm.removeComponent(selectedIndex);
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 1
                    color: ThemeColors.border
                }

                ToolButton {
                    text: qsTr("Customize")
                    Layout.fillWidth: true
                    enabled: selectedIndex >= 0
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Customize selected component.")
                    onClicked: {
                        if (vm)
                            vm.customizeComponent(selectedIndex);
                    }
                }

                ToolButton {
                    text: qsTr("Edit")
                    Layout.fillWidth: true
                    enabled: selectedIndex >= 0
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Edit selected component.")
                    onClicked: {
                        if (vm)
                            vm.editComponent(selectedIndex);
                    }
                }

                ToolButton {
                    text: qsTr("Revert")
                    Layout.fillWidth: true
                    enabled: selectedIndex >= 0
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Revert the selected component to default.")
                    onClicked: {
                        if (vm)
                            vm.revertComponent(selectedIndex);
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 1
                    color: ThemeColors.border
                }

                ToolButton {
                    text: qsTr("Install Loader")
                    Layout.fillWidth: true
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Install a mod loader.")
                    onClicked: installLoaderDialog.open()
                }

                ToolButton {
                    text: qsTr("Add to Minecraft.jar")
                    Layout.fillWidth: true
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Add a mod into the Minecraft jar file.")
                    onClicked: {
                        if (vm)
                            vm.addToMinecraftJar();
                    }
                }

                ToolButton {
                    text: qsTr("Replace Minecraft.jar")
                    Layout.fillWidth: true
                    onClicked: {
                        if (vm)
                            vm.replaceMinecraftJar();
                    }
                }

                ToolButton {
                    text: qsTr("Add Agents")
                    Layout.fillWidth: true
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Add Java agents.")
                    onClicked: {
                        if (vm)
                            vm.addAgents();
                    }
                }

                ToolButton {
                    text: qsTr("Add Empty")
                    Layout.fillWidth: true
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Add an empty custom component.")
                    onClicked: {
                        if (vm)
                            vm.addEmptyComponent();
                    }
                }

                ToolButton {
                    text: qsTr("Import Components")
                    Layout.fillWidth: true
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Import existing component JSON files.")
                    onClicked: {
                        if (vm)
                            vm.importComponents();
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 1
                    color: ThemeColors.border
                }

                ToolButton {
                    text: qsTr("Open .minecraft")
                    Layout.fillWidth: true
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Open the instance's .minecraft folder.")
                    onClicked: {
                        if (vm)
                            vm.openMinecraftFolder();
                    }
                }

                ToolButton {
                    text: qsTr("Open libraries")
                    Layout.fillWidth: true
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Open the instance's local libraries folder.")
                    onClicked: {
                        if (vm)
                            vm.openLibrariesFolder();
                    }
                }

                ToolButton {
                    text: qsTr("Reload")
                    Layout.fillWidth: true
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Reload all components.")
                    onClicked: {
                        if (vm)
                            vm.reloadComponents();
                    }
                }

                ToolButton {
                    text: qsTr("Download all")
                    Layout.fillWidth: true
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Download the files needed to launch the instance now.")
                    onClicked: {
                        if (vm)
                            vm.downloadAll();
                    }
                }

                Item {
                    Layout.fillHeight: true
                }
            }
        }
    }

    // Change version dialog
    Dialog {
        id: changeVersionDialog
        title: qsTr("Change Minecraft Version")
        modal: true
        standardButtons: Dialog.Ok | Dialog.Cancel
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        width: 500
        height: 400

        ColumnLayout {
            anchors.fill: parent
            spacing: Theme.spacingS

            RowLayout {
                Layout.fillWidth: true

                CheckBox {
                    id: releasesCheck
                    text: qsTr("Releases")
                    checked: true
                }
                CheckBox {
                    id: snapshotsCheck
                    text: qsTr("Snapshots")
                }
                CheckBox {
                    id: oldVersionsCheck
                    text: qsTr("Old versions")
                }
            }

            ListView {
                id: versionsList
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                model: vm ? vm.availableMinecraftVersions : []

                delegate: ItemDelegate {
                    width: versionsList.width
                    height: 32
                    highlighted: ListView.isCurrentItem
                    text: modelData
                    onClicked: versionsList.currentIndex = index
                }

                ScrollBar.vertical: ScrollBar {}
            }
        }

        onAccepted: {
            if (vm && versionsList.currentIndex >= 0) {
                var version = vm.availableMinecraftVersions[versionsList.currentIndex];
                vm.changeMinecraftVersion(version);
            }
        }
    }

    // Install loader dialog
    Dialog {
        id: installLoaderDialog
        title: qsTr("Install Mod Loader")
        modal: true
        standardButtons: Dialog.Cancel
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        width: 400

        ColumnLayout {
            anchors.fill: parent
            spacing: Theme.spacingM

            Label {
                text: qsTr("Select a mod loader to install:")
                color: ThemeColors.text
            }

            Repeater {
                model: ["Forge", "Fabric", "Quilt", "NeoForge", "LiteLoader"]

                Button {
                    text: modelData
                    Layout.fillWidth: true
                    onClicked: {
                        installLoaderDialog.close();
                        if (vm)
                            vm.installModLoader(modelData.toLowerCase());
                    }
                }
            }
        }
    }
}
