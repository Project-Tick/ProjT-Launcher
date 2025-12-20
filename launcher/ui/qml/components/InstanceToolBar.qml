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

Rectangle {
    id: instanceToolbar
    width: 120

    // Theme binding - directly from themeVM for reliable updates
    property var themeVM: ProjT.themeVM
    property int _themeUpdateCount: 0

    // Computed colors for theme reactivity
    color: {
        var _ = _themeUpdateCount;
        return themeVM ? Qt.darker(themeVM.windowColor, 1.05) : ThemeColors.toolBar;
    }

    property color toolBarColor: {
        var _ = _themeUpdateCount;
        return themeVM ? Qt.darker(themeVM.windowColor, 1.05) : ThemeColors.toolBar;
    }
    property color borderColor: {
        var _ = _themeUpdateCount;
        return themeVM ? Qt.darker(themeVM.windowColor, 1.2) : ThemeColors.border;
    }

    // Listen for theme changes
    Connections {
        target: themeVM
        function onThemeColorsChanged() {
            console.log("[InstanceToolBar] Theme colors changed");
            instanceToolbar._themeUpdateCount++;
        }
    }

    // ViewModel reference
    readonly property var vm: ProjT.instancesVM

    // Instance state
    readonly property bool hasSelection: vm && vm.selectedInstanceId.length > 0
    readonly property bool isRunning: vm && vm.isSelectedRunning
    readonly property bool canLaunch: hasSelection && !isRunning

    // Signals (matching ShellRoot.qml handler names)
    signal editInstance
    signal changeGroup
    signal exportInstance
    signal manageBackups
    signal copyInstance
    signal deleteInstance
    signal createShortcut

    Rectangle {
        anchors.fill: parent
        color: instanceToolbar.toolBarColor

        // Left border only
        Rectangle {
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: 1
            color: instanceToolbar.borderColor
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: Theme.spacingS
            spacing: 4

            // === Launch / Kill Button ===
            ThemedToolButton {
                text: isRunning ? qsTr("Kill") : qsTr("Launch")
                icon.name: isRunning ? "process-stop" : "media-playback-start"
                display: AbstractButton.TextBesideIcon
                Layout.fillWidth: true
                Layout.preferredHeight: 34
                enabled: hasSelection

                danger: isRunning
                success: !isRunning && hasSelection

                onClicked: {
                    if (isRunning && vm) {
                        vm.killSelectedInstance();
                    } else if (vm) {
                        vm.launchSelectedInstance();
                    }
                }

                ToolTip.text: isRunning ? qsTr("Kill the running instance") : qsTr("Launch the selected instance")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }

            // Separator
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 1
                Layout.topMargin: 4
                Layout.bottomMargin: 4
                color: instanceToolbar.borderColor
            }

            // === Edit Button ===
            ThemedToolButton {
                text: qsTr("Edit...")
                icon.name: "configure"
                display: AbstractButton.TextBesideIcon
                Layout.fillWidth: true
                Layout.preferredHeight: 28
                enabled: hasSelection

                onClicked: instanceToolbar.editInstance()

                ToolTip.text: qsTr("Change the instance settings, mods and versions")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }

            // === Change Group Button ===
            ThemedToolButton {
                text: qsTr("Change Group...")
                icon.name: "tag"
                display: AbstractButton.TextBesideIcon
                Layout.fillWidth: true
                Layout.preferredHeight: 28
                enabled: hasSelection

                onClicked: instanceToolbar.changeGroup()

                ToolTip.text: qsTr("Change the selected instance's group")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }

            // === Folder Button ===
            ThemedToolButton {
                text: qsTr("Folder")
                icon.name: "folder"
                display: AbstractButton.TextBesideIcon
                Layout.fillWidth: true
                Layout.preferredHeight: 28
                enabled: hasSelection

                onClicked: {
                    if (vm)
                        vm.openInstanceFolder();
                }

                ToolTip.text: qsTr("Open the selected instance's root folder")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }

            // === Export Button ===
            ThemedToolButton {
                text: qsTr("Export...")
                icon.name: "document-export"
                display: AbstractButton.TextBesideIcon
                Layout.fillWidth: true
                Layout.preferredHeight: 28
                enabled: hasSelection && !isRunning

                onClicked: instanceToolbar.exportInstance()

                ToolTip.text: qsTr("Export the selected instance")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }

            // === Manage Backups Button ===
            ThemedToolButton {
                text: qsTr("Backups...")
                icon.name: "document-save"
                display: AbstractButton.TextBesideIcon
                Layout.fillWidth: true
                Layout.preferredHeight: 28
                enabled: hasSelection

                onClicked: instanceToolbar.manageBackups()

                ToolTip.text: qsTr("Create, restore, and manage instance backups")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }

            // === Copy Button ===
            ThemedToolButton {
                text: qsTr("Copy...")
                icon.name: "edit-copy"
                display: AbstractButton.TextBesideIcon
                Layout.fillWidth: true
                Layout.preferredHeight: 28
                enabled: hasSelection && !isRunning

                onClicked: instanceToolbar.copyInstance()

                ToolTip.text: qsTr("Copy the selected instance")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }

            // === Delete Button ===
            ThemedToolButton {
                text: qsTr("Delete")
                icon.name: "edit-delete"
                display: AbstractButton.TextBesideIcon
                Layout.fillWidth: true
                Layout.preferredHeight: 28
                enabled: hasSelection && !isRunning
                danger: true

                onClicked: instanceToolbar.deleteInstance()

                ToolTip.text: qsTr("Delete the selected instance")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }

            // === Create Shortcut Button ===
            ThemedToolButton {
                text: qsTr("Shortcut")
                icon.name: "link"
                display: AbstractButton.TextBesideIcon
                Layout.fillWidth: true
                Layout.preferredHeight: 28
                enabled: hasSelection

                onClicked: instanceToolbar.createShortcut()

                ToolTip.text: qsTr("Create a shortcut to launch this instance")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }

            // === Spacer ===
            Item {
                Layout.fillHeight: true
            }
        }
    }
}
