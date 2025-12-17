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

Dialog {
    id: newInstanceDialog
    title: qsTr("New Instance")
    modal: true
    width: 700
    height: 550
    standardButtons: Dialog.NoButton

    property var vm: ProjT ? ProjT.instancesVM : null
    property string currentPage: "vanilla"

    // Theme binding for reactive updates
    property var themeVM: ProjT.themeVM
    property int _themeUpdateCount: 0

    Connections {
        target: themeVM
        function onThemeColorsChanged() {
            newInstanceDialog._themeUpdateCount++;
        }
    }

    // Dialog background styling
    background: Rectangle {
        color: ThemeColors.background
        border.color: ThemeColors.border
        border.width: 1
        radius: 8
    }

    // Dialog header styling
    header: Rectangle {
        height: 40
        color: ThemeColors.toolBar

        Label {
            anchors.centerIn: parent
            text: newInstanceDialog.title
            color: ThemeColors.text
            font.bold: true
            font.pointSize: 12
        }

        Rectangle {
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            height: 1
            color: ThemeColors.border
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        // Left sidebar
        Rectangle {
            Layout.preferredWidth: 180
            Layout.fillHeight: true
            color: ThemeColors.backgroundAlt

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: Theme.spacingS
                spacing: 2

                Label {
                    text: qsTr("Create Instance")
                    color: ThemeColors.text
                    font.bold: true
                    Layout.fillWidth: true
                    padding: Theme.spacingS
                }

                // Navigation items
                Repeater {
                    model: [
                        {
                            id: "vanilla",
                            name: qsTr("Vanilla"),
                            icon: "🎮"
                        },
                        {
                            id: "curseforge",
                            name: "CurseForge",
                            icon: "🔥"
                        },
                        {
                            id: "modrinth",
                            name: "Modrinth",
                            icon: "🌿"
                        },
                        {
                            id: "atlauncher",
                            name: "ATLauncher",
                            icon: "⚡"
                        },
                        {
                            id: "ftb",
                            name: "FTB",
                            icon: "📦"
                        },
                        {
                            id: "technic",
                            name: "Technic",
                            icon: "⚙"
                        },
                        {
                            id: "import",
                            name: qsTr("Import"),
                            icon: "📂"
                        }
                    ]

                    delegate: ItemDelegate {
                        Layout.fillWidth: true
                        highlighted: currentPage === modelData.id

                        background: Rectangle {
                            color: highlighted ? Qt.rgba(ThemeColors.highlight.r, ThemeColors.highlight.g, ThemeColors.highlight.b, 0.2) : (hovered ? ThemeColors.hover : "transparent")
                            radius: 4
                        }

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: Theme.spacingS
                            spacing: Theme.spacingS

                            Label {
                                text: modelData.icon
                                font.pointSize: 14
                            }

                            Label {
                                text: modelData.name
                                color: highlighted ? ThemeColors.highlight : ThemeColors.text
                                font.bold: highlighted
                            }
                        }

                        onClicked: currentPage = modelData.id
                    }
                }

                Item {
                    Layout.fillHeight: true
                }
            }
        }

        // Main content
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: ThemeColors.background

            StackLayout {
                anchors.fill: parent
                anchors.margins: Theme.spacingM
                currentIndex: {
                    switch (currentPage) {
                    case "vanilla":
                        return 0;
                    case "curseforge":
                        return 1;
                    case "modrinth":
                        return 2;
                    case "atlauncher":
                        return 3;
                    case "ftb":
                        return 4;
                    case "technic":
                        return 5;
                    case "import":
                        return 6;
                    default:
                        return 0;
                    }
                }

                // Vanilla page
                VanillaPage {}

                // CurseForge placeholder
                ModpackBrowserPage {
                    source: "curseforge"
                    sourceName: "CurseForge"
                    sourceColor: ThemeColors.error
                }

                // Modrinth placeholder
                ModpackBrowserPage {
                    source: "modrinth"
                    sourceName: "Modrinth"
                    sourceColor: "#1bd96a"
                }

                // ATLauncher placeholder
                ModpackBrowserPage {
                    source: "atlauncher"
                    sourceName: "ATLauncher"
                    sourceColor: "#1e8e3e"
                }

                // FTB placeholder
                ModpackBrowserPage {
                    source: "ftb"
                    sourceName: "FTB"
                    sourceColor: ThemeColors.error
                }

                // Technic placeholder
                ModpackBrowserPage {
                    source: "technic"
                    sourceName: "Technic"
                    sourceColor: ThemeColors.error
                }

                // Import page
                ImportPage {}
            }
        }
    }

    // Vanilla instance creation
    component VanillaPage: ColumnLayout {
        spacing: Theme.spacingM

        Label {
            text: qsTr("Create Vanilla Instance")
            color: ThemeColors.text
            font.bold: true
            font.pointSize: Theme.fontSizeMedium
        }

        // Instance name
        RowLayout {
            Layout.fillWidth: true

            Label {
                text: qsTr("Name:")
                Layout.preferredWidth: 100
            }

            TextField {
                id: vanillaNameField
                Layout.fillWidth: true
                placeholderText: qsTr("Instance name...")
            }
        }

        // Group
        RowLayout {
            Layout.fillWidth: true

            Label {
                text: qsTr("Group:")
                Layout.preferredWidth: 100
            }

            ComboBox {
                id: vanillaGroupCombo
                Layout.fillWidth: true
                editable: true
                model: vm ? vm.groupList : []
            }
        }

        // Version selection
        GroupBox {
            Layout.fillWidth: true
            Layout.fillHeight: true
            title: qsTr("Minecraft Version")

            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS

                RowLayout {
                    Layout.fillWidth: true

                    CheckBox {
                        id: showReleasesCheck
                        text: qsTr("Releases")
                        checked: true
                    }

                    CheckBox {
                        id: showSnapshotsCheck
                        text: qsTr("Snapshots")
                    }

                    CheckBox {
                        id: showOldCheck
                        text: qsTr("Old Versions")
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    ThemedButton {
                        text: qsTr("Refresh")
                        icon.name: "view-refresh"
                        size: "small"
                        onClicked: {
                            if (vm)
                                vm.refreshVersions();
                        }
                    }
                }

                TextField {
                    id: versionSearchField
                    Layout.fillWidth: true
                    placeholderText: qsTr("Search versions...")
                }

                Frame {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    ListView {
                        id: versionList
                        anchors.fill: parent
                        clip: true
                        model: vm ? vm.minecraftVersions : []

                        property string selectedVersion: ""

                        delegate: ItemDelegate {
                            width: versionList.width
                            height: visible ? 36 : 0
                            highlighted: modelData === versionList.selectedVersion

                            visible: {
                                // Filter by search
                                if (versionSearchField.text.length > 0) {
                                    if (!modelData.toLowerCase().includes(versionSearchField.text.toLowerCase())) {
                                        return false;
                                    }
                                }
                                return true;
                            }

                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: Theme.spacingS

                                Rectangle {
                                    Layout.preferredWidth: 20
                                    Layout.preferredHeight: 20
                                    radius: 4
                                    color: {
                                        if (modelData.includes("snapshot") || modelData.includes("pre") || modelData.includes("rc"))
                                            return ThemeColors.warning;
                                        if (modelData.startsWith("b") || modelData.startsWith("a"))
                                            return "#8b5cf6";
                                        return ThemeColors.success;
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
                                        font.pointSize: 9
                                    }
                                }

                                Label {
                                    text: modelData
                                    color: ThemeColors.text
                                    Layout.fillWidth: true
                                }
                            }

                            onClicked: versionList.selectedVersion = modelData
                        }

                        ScrollBar.vertical: ScrollBar {}
                    }
                }
            }
        }

        // Create button
        RowLayout {
            Layout.fillWidth: true

            Item {
                Layout.fillWidth: true
            }

            ThemedButton {
                text: qsTr("Cancel")
                onClicked: newInstanceDialog.reject()
            }

            ThemedButton {
                text: qsTr("Create")
                primary: true
                enabled: vanillaNameField.text.length > 0 && versionList.selectedVersion.length > 0
                onClicked: {
                    if (vm) {
                        vm.createNewInstance(vanillaNameField.text, vanillaGroupCombo.editText, versionList.selectedVersion);
                        newInstanceDialog.accept();
                    }
                }
            }
        }
    }

    // Modpack browser page component
    component ModpackBrowserPage: ColumnLayout {
        property string source: ""
        property string sourceName: ""
        property string sourceColor: ThemeColors.accent

        spacing: Theme.spacingM

        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingM

            Rectangle {
                Layout.preferredWidth: 40
                Layout.preferredHeight: 40
                radius: 8
                color: sourceColor

                Label {
                    anchors.centerIn: parent
                    text: sourceName.charAt(0)
                    color: "white"
                    font.bold: true
                    font.pointSize: 16
                }
            }

            Label {
                text: qsTr("Browse %1 Modpacks").arg(sourceName)
                color: ThemeColors.text
                font.bold: true
                font.pointSize: Theme.fontSizeMedium
            }
        }

        // Search
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingS

            TextField {
                id: modpackSearchField
                Layout.fillWidth: true
                placeholderText: qsTr("Search modpacks...")
            }

            ThemedButton {
                text: qsTr("Search")
                primary: true
            }
        }

        // Results placeholder
        Frame {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Label {
                anchors.centerIn: parent
                text: qsTr("Search for %1 modpacks to install").arg(sourceName)
                color: ThemeColors.textSecondary
            }
        }

        // Buttons
        RowLayout {
            Layout.fillWidth: true

            Item {
                Layout.fillWidth: true
            }

            ThemedButton {
                text: qsTr("Cancel")
                onClicked: newInstanceDialog.reject()
            }
        }
    }

    // Import page component
    component ImportPage: ColumnLayout {
        spacing: Theme.spacingM

        Label {
            text: qsTr("Import Instance")
            color: ThemeColors.text
            font.bold: true
            font.pointSize: Theme.fontSizeMedium
        }

        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Import from File")

            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS

                Label {
                    Layout.fillWidth: true
                    text: qsTr("Import a modpack from a local file (.zip, .mrpack, or folder)")
                    color: ThemeColors.textSecondary
                    wrapMode: Text.WordWrap
                }

                RowLayout {
                    Layout.fillWidth: true

                    TextField {
                        id: importPathField
                        Layout.fillWidth: true
                        placeholderText: qsTr("Select a file or folder...")
                        readOnly: true
                    }

                    ThemedButton {
                        text: qsTr("Browse...")
                        onClicked: openImportFileDialog()
                    }
                }
            }
        }

        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Import from URL")

            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS

                Label {
                    Layout.fillWidth: true
                    text: qsTr("Import a modpack from a URL (CurseForge, Modrinth, etc.)")
                    color: ThemeColors.textSecondary
                    wrapMode: Text.WordWrap
                }

                TextField {
                    id: importUrlField
                    Layout.fillWidth: true
                    placeholderText: qsTr("https://...")
                }
            }
        }

        // Instance settings
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Instance Settings")

            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS

                RowLayout {
                    Layout.fillWidth: true

                    Label {
                        text: qsTr("Name:")
                        Layout.preferredWidth: 80
                    }

                    TextField {
                        id: importNameField
                        Layout.fillWidth: true
                        placeholderText: qsTr("Instance name (leave empty for auto)")
                    }
                }

                RowLayout {
                    Layout.fillWidth: true

                    Label {
                        text: qsTr("Group:")
                        Layout.preferredWidth: 80
                    }

                    ComboBox {
                        id: importGroupCombo
                        Layout.fillWidth: true
                        editable: true
                        model: vm ? vm.groupList : []
                    }
                }
            }
        }

        Item {
            Layout.fillHeight: true
        }

        // Buttons
        RowLayout {
            Layout.fillWidth: true

            Item {
                Layout.fillWidth: true
            }

            ThemedButton {
                text: qsTr("Cancel")
                onClicked: newInstanceDialog.reject()
            }

            ThemedButton {
                text: qsTr("Import")
                primary: true
                enabled: importPathField.text.length > 0 || importUrlField.text.length > 0
                onClicked: {
                    if (vm) {
                        if (importUrlField.text.length > 0) {
                            vm.importFromUrl(importUrlField.text, importNameField.text, importGroupCombo.editText);
                        } else {
                            vm.importFromFile(importPathField.text, importNameField.text, importGroupCombo.editText);
                        }
                        newInstanceDialog.accept();
                    }
                }
            }
        }

        // Use ViewModel to open file dialog (Qt 6 compatible)
        function openImportFileDialog() {
            if (ProjT && ProjT.launcherVM) {
                var path = ProjT.launcherVM.browseForFile(qsTr("Select Modpack"), "Modpack files (*.zip *.mrpack);;All files (*)");
                if (path && path.length > 0) {
                    importPathField.text = path;
                }
            }
        }
    }
}
