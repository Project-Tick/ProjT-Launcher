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
    id: customPage
    color: ThemeColors.background

    property string selectedVersion: ""
    property var vm: ProjT.newInstanceVM

    // Signals for parent communication
    signal versionSelected(string version)
    signal createRequested(string instanceName, string version, string loader, string loaderVersion)

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingM
        spacing: Theme.spacingM

        // Header
        Label {
            text: qsTr("Custom")
            font.pointSize: 14
            font.bold: true
            color: ThemeColors.text
        }

        Label {
            text: qsTr("Select a Minecraft version to create a new instance")
            color: ThemeColors.textSecondary
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }

        // Filter options
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Filter")

            RowLayout {
                anchors.fill: parent
                spacing: Theme.spacingM

                CheckBox {
                    id: showReleasesCheck
                    text: qsTr("Releases")
                    checked: vm ? vm.showReleases : true
                    onCheckedChanged: {
                        if (vm)
                            vm.showReleases = checked;
                        filterVersions();
                    }
                }

                CheckBox {
                    id: showSnapshotsCheck
                    text: qsTr("Snapshots")
                    checked: vm ? vm.showSnapshots : false
                    onCheckedChanged: {
                        if (vm)
                            vm.showSnapshots = checked;
                        filterVersions();
                    }
                }

                CheckBox {
                    id: showBetasCheck
                    text: qsTr("Old Versions")
                    checked: vm ? vm.showOldVersions : false
                    onCheckedChanged: {
                        if (vm)
                            vm.showOldVersions = checked;
                        filterVersions();
                    }
                }

                Item {
                    Layout.fillWidth: true
                }

                Button {
                    text: qsTr("Refresh")
                    icon.name: "view-refresh"
                    onClicked: {
                        if (vm) {
                            vm.loadMinecraftVersions();
                        }
                    }
                }
            }
        }

        // Version list
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: ThemeColors.surface
            border.color: ThemeColors.border
            border.width: 1
            radius: Theme.radiusS

            ListView {
                id: versionListView
                anchors.fill: parent
                anchors.margins: 1
                clip: true

                model: vm ? vm.minecraftVersionsModel : []

                delegate: ItemDelegate {
                    width: versionListView.width
                    height: 32
                    highlighted: versionListView.currentIndex === index

                    contentItem: RowLayout {
                        spacing: Theme.spacingS

                        Image {
                            Layout.preferredWidth: 16
                            Layout.preferredHeight: 16
                            source: Theme.versionTypeIcon(model.type || "release")
                            fillMode: Image.PreserveAspectFit
                        }

                        Label {
                            text: model.version || model.versionId || ""
                            color: ThemeColors.text
                            Layout.fillWidth: true
                        }

                        Label {
                            text: model.type || ""
                            color: ThemeColors.textSecondary
                            font.pointSize: 9
                        }
                    }

                    onClicked: {
                        versionListView.currentIndex = index;
                        var version = model.version || model.versionId || "";
                        customPage.selectedVersion = version;
                        if (vm)
                            vm.selectedMinecraftVersion = version;
                        versionSelected(version);
                    }

                    onDoubleClicked: {
                        versionListView.currentIndex = index;
                        var version = model.version || model.versionId || "";
                        customPage.selectedVersion = version;
                        if (vm)
                            vm.selectedMinecraftVersion = version;
                        versionSelected(version);
                    }
                }

                ScrollBar.vertical: ScrollBar {}
            }

            // Empty state
            Label {
                anchors.centerIn: parent
                visible: versionListView.count === 0
                text: qsTr("Loading versions...")
                color: ThemeColors.textSecondary
            }
        }

        // Loader selection
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Mod Loader (optional)")

            RowLayout {
                anchors.fill: parent
                spacing: Theme.spacingM

                ComboBox {
                    id: loaderCombo
                    Layout.preferredWidth: 150
                    model: ["None", "Forge", "Fabric", "Quilt", "NeoForge", "LiteLoader"]
                    onCurrentTextChanged: {
                        if (vm)
                            vm.selectedModLoader = currentText;
                    }
                }

                ComboBox {
                    id: loaderVersionCombo
                    Layout.fillWidth: true
                    enabled: loaderCombo.currentIndex > 0
                    model: vm ? vm.modLoaderVersionsModel : []
                    onCurrentTextChanged: {
                        if (vm)
                            vm.selectedModLoaderVersion = currentText;
                    }

                    Label {
                        anchors.centerIn: parent
                        visible: parent.count === 0 && parent.enabled
                        text: qsTr("Select version first")
                        color: ThemeColors.textSecondary
                    }
                }
            }
        }

        // Selected version info
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Selected Version")
            visible: customPage.selectedVersion !== ""

            RowLayout {
                anchors.fill: parent
                spacing: Theme.spacingM

                Image {
                    Layout.preferredWidth: 24
                    Layout.preferredHeight: 24
                    source: Theme.icon("minecraft")
                    fillMode: Image.PreserveAspectFit
                }

                Label {
                    text: customPage.selectedVersion
                    color: ThemeColors.text
                    font.bold: true
                    Layout.fillWidth: true
                }

                Label {
                    text: loaderCombo.currentIndex > 0 ? ("+ " + loaderCombo.currentText) : ""
                    color: ThemeColors.accent
                    visible: loaderCombo.currentIndex > 0
                }
            }
        }
    }

    // Loading overlay
    Rectangle {
        anchors.fill: parent
        color: Qt.rgba(0, 0, 0, 0.5)
        visible: vm ? vm.isLoading : false

        Column {
            anchors.centerIn: parent
            spacing: Theme.spacingM

            BusyIndicator {
                anchors.horizontalCenter: parent.horizontalCenter
                running: parent.visible
            }

            Label {
                text: qsTr("Creating instance...")
                color: "white"
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }

    function filterVersions() {
        if (vm) {
            vm.filterVersions();
        }
    }

    Component.onCompleted: {
        if (vm) {
            vm.loadMinecraftVersions();
        }
    }

    Connections {
        target: vm
        function onMinecraftVersionsModelChanged() {
            console.log("Versions loaded");
        }
        function onInstanceCreationFinished(success, message) {
            if (success) {
                console.log("Instance created: " + message);
            } else {
                console.log("Creation failed: " + message);
            }
        }
    }
}
