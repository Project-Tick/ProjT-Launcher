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
    id: root
    objectName: "worldsPage"
    color: ThemeColors.background

    property var vm: ProjT.instanceVM

    // Handle worldImportRequested signal from ViewModel
    Connections {
        target: root.vm
        function onWorldImportRequested() {
            // Open file dialog for world import
            if (ProjT && ProjT.launcherVM && ProjT.launcherVM.browseForFile) {
                var filter = qsTr("World files (*.zip);;World folders (*)");
                var path = ProjT.launcherVM.browseForFile(qsTr("Import World"), filter);
                if (path && path.length > 0 && root.vm) {
                    root.vm.importWorldFromPath(path);
                }
            }
        }

        function onWorldBackupCompleted(success, backupPath) {
            if (success) {
                backupSuccessDialog.backupPath = backupPath;
                backupSuccessDialog.open();
            } else {
                backupFailedDialog.open();
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingM
        spacing: Theme.spacingM

        // Header
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingM

            Text {
                text: qsTr("Worlds")
                font.pixelSize: 24
                font.weight: Font.Bold
                color: ThemeColors.text
            }

            Item {
                Layout.fillWidth: true
            }

            Button {
                text: qsTr("Open Folder")
                onClicked: if (root.vm)
                    root.vm.openGameFolder()

                background: Rectangle {
                    color: parent.hovered ? ThemeColors.hover : ThemeColors.surface
                    border.color: ThemeColors.border
                    border.width: 1
                    radius: Theme.radiusS
                }

                contentItem: Text {
                    text: parent.text
                    color: ThemeColors.text
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }

        Text {
            text: root.vm && root.vm.instanceName ? root.vm.instanceName : qsTr("No instance selected")
            font.pixelSize: 14
            color: ThemeColors.textSecondary
            visible: root.vm !== null
        }

        // Toolbar
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingS

            Button {
                text: qsTr("Add")
                onClicked: {
                    if (root.vm)
                        root.vm.importWorld();
                }

                background: Rectangle {
                    color: parent.hovered ? ThemeColors.hover : ThemeColors.surface
                    border.color: ThemeColors.border
                    border.width: 1
                    radius: Theme.radiusS
                }

                contentItem: Text {
                    text: parent.text
                    color: ThemeColors.text
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }

            Button {
                text: qsTr("Copy")
                enabled: worldsList.currentIndex >= 0
                onClicked: {
                    if (root.vm && worldsList.currentIndex >= 0) {
                        root.vm.copyWorld(worldsList.currentIndex);
                    }
                }

                background: Rectangle {
                    color: parent.enabled ? (parent.hovered ? ThemeColors.hover : ThemeColors.surface) : ThemeColors.disabled
                    border.color: ThemeColors.border
                    border.width: 1
                    radius: Theme.radiusS
                }

                contentItem: Text {
                    text: parent.text
                    color: parent.enabled ? ThemeColors.text : ThemeColors.textSecondary
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }

            Button {
                text: qsTr("Backup")
                enabled: worldsList.currentIndex >= 0
                onClicked: {
                    if (root.vm && worldsList.currentIndex >= 0) {
                        root.vm.backupWorld(worldsList.currentIndex);
                    }
                }

                background: Rectangle {
                    color: parent.enabled ? (parent.hovered ? ThemeColors.hover : ThemeColors.surface) : ThemeColors.disabled
                    border.color: ThemeColors.border
                    border.width: 1
                    radius: Theme.radiusS
                }

                contentItem: Text {
                    text: parent.text
                    color: parent.enabled ? ThemeColors.text : ThemeColors.textSecondary
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }

            Button {
                text: qsTr("Delete")
                enabled: worldsList.currentIndex >= 0
                onClicked: deleteWorldDialog.open()

                background: Rectangle {
                    color: parent.enabled ? (parent.hovered ? ThemeColors.error : ThemeColors.surface) : ThemeColors.disabled
                    border.color: parent.enabled ? ThemeColors.error : ThemeColors.border
                    border.width: 1
                    radius: Theme.radiusS
                }

                contentItem: Text {
                    text: parent.text
                    color: parent.enabled ? (parent.hovered ? ThemeColors.surface : ThemeColors.error) : ThemeColors.textSecondary
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }

            Item {
                Layout.fillWidth: true
            }

            Button {
                text: qsTr("Refresh")
                onClicked: {
                    if (root.vm)
                        root.vm.refreshWorlds();
                }

                background: Rectangle {
                    color: parent.hovered ? ThemeColors.hover : ThemeColors.surface
                    border.color: ThemeColors.border
                    border.width: 1
                    radius: Theme.radiusS
                }

                contentItem: Text {
                    text: parent.text
                    color: ThemeColors.text
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }

        // Worlds List
        ListView {
            id: worldsList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: Theme.spacingXS

            // Use ViewModel's world paths
            model: root.vm ? root.vm.worldPaths : []

            Component.onCompleted: {
                if (root.vm)
                    root.vm.refreshWorlds();
            }

            // Empty state
            Text {
                anchors.centerIn: parent
                text: qsTr("No worlds found.\nCreate or import worlds to see them here.")
                horizontalAlignment: Text.AlignHCenter
                color: ThemeColors.textSecondary
                font.pixelSize: 14
                visible: worldsList.count === 0
            }

            delegate: Rectangle {
                width: worldsList.width
                height: 80
                color: worldsList.currentIndex === index ? ThemeColors.highlight : ThemeColors.surface
                border.color: worldsList.currentIndex === index ? ThemeColors.accent : ThemeColors.hover
                border.width: 1
                radius: Theme.radiusS

                property string worldPath: modelData
                property string worldName: root.vm && root.vm.worldNames[index] ? root.vm.worldNames[index] : ""

                MouseArea {
                    anchors.fill: parent
                    onClicked: worldsList.currentIndex = index
                    onDoubleClicked: {
                        // Open world folder
                        if (root.vm)
                            root.vm.openWorldFolder(index);
                    }
                }

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: Theme.spacingS
                    spacing: Theme.spacingM

                    // World icon placeholder
                    Rectangle {
                        width: 64
                        height: 64
                        color: ThemeColors.disabled
                        radius: Theme.radiusS

                        Image {
                            anchors.fill: parent
                            anchors.margins: 4
                            source: model.icon || ""
                            fillMode: Image.PreserveAspectFit
                            visible: source !== ""
                        }

                        Text {
                            anchors.centerIn: parent
                            text: "🌍"
                            font.pixelSize: 32
                        }
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        spacing: 2

                        Text {
                            text: worldName || qsTr("World %1").arg(index + 1)
                            color: ThemeColors.text
                            font.weight: Font.DemiBold
                            font.pixelSize: 14
                        }

                        Text {
                            text: qsTr("Survival")
                            color: ThemeColors.accent
                            font.pixelSize: 12
                        }
                    }
                }
            }
        }
    }

    // Delete World Dialog
    Dialog {
        id: deleteWorldDialog
        title: qsTr("Delete World")
        modal: true
        standardButtons: Dialog.Yes | Dialog.No
        x: (root.width - width) / 2
        y: (root.height - height) / 2

        Label {
            text: qsTr("Are you sure you want to delete this world?\n\nThis action cannot be undone!")
            color: ThemeColors.error
            wrapMode: Text.WordWrap
        }

        onAccepted: {
            if (root.vm && worldsList.currentIndex >= 0) {
                root.vm.deleteWorld(worldsList.currentIndex);
            }
        }
    }

    // Backup Success Dialog
    Dialog {
        id: backupSuccessDialog
        title: qsTr("Backup Created")
        modal: true
        standardButtons: Dialog.Ok
        x: (root.width - width) / 2
        y: (root.height - height) / 2

        property string backupPath: ""

        Label {
            text: qsTr("World backup created successfully!\n\nSaved to: %1").arg(backupSuccessDialog.backupPath)
            color: ThemeColors.text
            wrapMode: Text.WordWrap
        }
    }

    // Backup Failed Dialog
    Dialog {
        id: backupFailedDialog
        title: qsTr("Backup Failed")
        modal: true
        standardButtons: Dialog.Ok
        x: (root.width - width) / 2
        y: (root.height - height) / 2

        Label {
            text: qsTr("Failed to create world backup. Please try again.")
            color: ThemeColors.error
            wrapMode: Text.WordWrap
        }
    }
}
