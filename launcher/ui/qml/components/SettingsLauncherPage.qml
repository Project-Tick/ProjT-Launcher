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
import "."

ScrollView {
    id: launcherPage
    clip: true
    contentWidth: availableWidth
    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

    property var vm: ProjT.launcherSettingsVM

    Rectangle {
        width: launcherPage.availableWidth
        implicitHeight: mainColumn.implicitHeight + 40
        color: "transparent"

        ColumnLayout {
            id: mainColumn
            width: Math.min(parent.width - 40, 700)
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 20
            spacing: 16

            // === User Interface ===
            SettingsSection {
                Layout.fillWidth: true
                title: qsTr("User Interface")
                iconSource: Theme.icon("appearance")

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 16

                    // Instance Sorting
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 8
                        
                        Label {
                            text: qsTr("Instance Sorting")
                            color: ThemeColors.text
                            font.pixelSize: 13
                            font.weight: Font.Medium
                        }

                        RowLayout {
                            spacing: 24
                            
                            RadioButton {
                                id: sortByNameRadio
                                text: qsTr("By Name")
                                checked: vm ? vm.sortByName : true
                                onCheckedChanged: if (vm && checked) vm.sortByName = true
                                
                                indicator: Rectangle {
                                    implicitWidth: 20
                                    implicitHeight: 20
                                    x: sortByNameRadio.leftPadding
                                    y: parent.height / 2 - height / 2
                                    radius: 10
                                    color: "transparent"
                                    border.color: sortByNameRadio.checked ? ThemeColors.accent : ThemeColors.border
                                    border.width: 2
                                    
                                    Rectangle {
                                        width: 10
                                        height: 10
                                        anchors.centerIn: parent
                                        radius: 5
                                        color: ThemeColors.accent
                                        visible: sortByNameRadio.checked
                                    }
                                }
                                
                                contentItem: Text {
                                    text: sortByNameRadio.text
                                    color: ThemeColors.text
                                    font.pixelSize: 13
                                    leftPadding: sortByNameRadio.indicator.width + 8
                                    verticalAlignment: Text.AlignVCenter
                                }
                            }
                            
                            RadioButton {
                                id: sortByLastPlayedRadio
                                text: qsTr("By Last Played")
                                checked: vm ? !vm.sortByName : false
                                onCheckedChanged: if (vm && checked) vm.sortByName = false
                                
                                indicator: Rectangle {
                                    implicitWidth: 20
                                    implicitHeight: 20
                                    x: sortByLastPlayedRadio.leftPadding
                                    y: parent.height / 2 - height / 2
                                    radius: 10
                                    color: "transparent"
                                    border.color: sortByLastPlayedRadio.checked ? ThemeColors.accent : ThemeColors.border
                                    border.width: 2
                                    
                                    Rectangle {
                                        width: 10
                                        height: 10
                                        anchors.centerIn: parent
                                        radius: 5
                                        color: ThemeColors.accent
                                        visible: sortByLastPlayedRadio.checked
                                    }
                                }
                                
                                contentItem: Text {
                                    text: sortByLastPlayedRadio.text
                                    color: ThemeColors.text
                                    font.pixelSize: 13
                                    leftPadding: sortByLastPlayedRadio.indicator.width + 8
                                    verticalAlignment: Text.AlignVCenter
                                }
                            }
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 1
                        color: Qt.rgba(ThemeColors.separator.r, ThemeColors.separator.g, ThemeColors.separator.b, 0.5)
                    }

                    // Menubar toggle
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 12
                        
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2
                            
                            Label {
                                text: qsTr("Replace toolbar with menubar")
                                color: ThemeColors.text
                                font.pixelSize: 13
                            }
                            Label {
                                text: qsTr("Shows traditional menu bar instead of toolbar buttons")
                                color: ThemeColors.textSecondary
                                font.pixelSize: 11
                            }
                        }
                        
                        Switch {
                            checked: vm ? vm.preferMenuBar : false
                            onCheckedChanged: if (vm) vm.preferMenuBar = checked
                        }
                    }
                }
            }

            // === Updater ===
            SettingsSection {
                Layout.fillWidth: true
                title: qsTr("Updater")
                iconSource: Theme.icon("refresh")

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 16

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 12
                        
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2
                            
                            Label {
                                text: qsTr("Check for updates automatically")
                                color: ThemeColors.text
                                font.pixelSize: 13
                            }
                            Label {
                                text: qsTr("Launcher will check for new versions on startup")
                                color: ThemeColors.textSecondary
                                font.pixelSize: 11
                            }
                        }
                        
                        Switch {
                            checked: vm ? vm.autoUpdateCheck : true
                            onCheckedChanged: if (vm) vm.autoUpdateCheck = checked
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        visible: vm ? vm.autoUpdateCheck : true
                        spacing: 16
                        
                        Label { 
                            text: qsTr("Check Frequency:")
                            color: ThemeColors.textSecondary
                            font.pixelSize: 13
                        }
                        
                        SpinBox {
                            id: updateIntervalSpinner
                            from: 0
                            to: 168
                            stepSize: 1
                            value: vm ? vm.updateInterval : 0
                            editable: true
                            
                            textFromValue: function(value) {
                                if (value === 0) return qsTr("On Launch");
                                if (value === 1) return qsTr("1 hour");
                                return value + qsTr(" hours");
                            }
                            
                            valueFromText: function(text) {
                                return parseInt(text) || 0;
                            }
                            
                            onValueModified: if (vm) vm.updateInterval = value
                        }
                    }
                }
            }

            // === Storage Locations ===
            SettingsSection {
                Layout.fillWidth: true
                title: qsTr("Storage Locations")
                iconSource: Theme.icon("viewfolder")

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 12

                    Repeater {
                        model: [
                            { label: qsTr("Instances"), prop: "instancesFolder", browse: "browseForInstancesFolder" },
                            { label: qsTr("Mods"), prop: "modsFolder", browse: "browseForModsFolder" },
                            { label: qsTr("Icons"), prop: "iconsFolder", browse: "browseForIconsFolder" },
                            { label: qsTr("Java"), prop: "javaFolder", browse: "browseForJavaFolder" },
                            { label: qsTr("Downloads"), prop: "downloadsFolder", browse: "browseForDownloadsFolder" }
                        ]
                        
                        delegate: RowLayout {
                            Layout.fillWidth: true
                            spacing: 12
                            
                            Label {
                                text: modelData.label
                                color: ThemeColors.textSecondary
                                font.pixelSize: 13
                                Layout.preferredWidth: 100
                            }
                            
                            Rectangle {
                                Layout.fillWidth: true
                                height: 36
                                radius: 8
                                color: ThemeColors.bg1
                                border.color: ThemeColors.border
                                border.width: 1
                                
                                Text {
                                    anchors.fill: parent
                                    anchors.leftMargin: 12
                                    anchors.rightMargin: 12
                                    verticalAlignment: Text.AlignVCenter
                                    text: vm ? vm[modelData.prop] || "" : ""
                                    color: ThemeColors.text
                                    font.pixelSize: 12
                                    elide: Text.ElideMiddle
                                }
                            }
                            
                            ThemedButton {
                                text: qsTr("Browse")
                                onClicked: launcherPage[modelData.browse]()
                            }
                        }
                    }
                }
            }

            // === Mods & Modpacks ===
            SettingsSection {
                Layout.fillWidth: true
                title: qsTr("Mods & Modpacks")
                iconSource: Theme.icon("loadermods")

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 12

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 12
                        
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2
                            
                            Label {
                                text: qsTr("Check subfolders for blocked mods")
                                color: ThemeColors.text
                                font.pixelSize: 13
                            }
                            Label {
                                text: qsTr("Recursively watch downloads directory")
                                color: ThemeColors.textSecondary
                                font.pixelSize: 11
                            }
                        }
                        
                        Switch {
                            checked: vm ? vm.downloadsDirWatchRecursive : false
                            onCheckedChanged: if (vm) vm.downloadsDirWatchRecursive = checked
                        }
                    }

                    Rectangle { Layout.fillWidth: true; height: 1; color: Qt.rgba(ThemeColors.separator.r, ThemeColors.separator.g, ThemeColors.separator.b, 0.3) }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 12
                        
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2
                            
                            Label {
                                text: qsTr("Keep track of mod metadata")
                                color: ThemeColors.text
                                font.pixelSize: 13
                            }
                            Label {
                                text: qsTr("Required for automatic mod updates")
                                color: ThemeColors.textSecondary
                                font.pixelSize: 11
                            }
                        }
                        
                        Switch {
                            id: metadataSwitch
                            checked: vm ? vm.metadataEnabled : true
                            onCheckedChanged: if (vm) vm.metadataEnabled = checked
                        }
                    }
                    
                    // Warning message
                    Rectangle {
                        Layout.fillWidth: true
                        visible: vm && !vm.metadataEnabled
                        height: warningText.implicitHeight + 16
                        radius: 8
                        color: Qt.rgba(ThemeColors.warning.r, ThemeColors.warning.g, ThemeColors.warning.b, 0.1)
                        border.color: Qt.rgba(ThemeColors.warning.r, ThemeColors.warning.g, ThemeColors.warning.b, 0.3)
                        
                        Label {
                            id: warningText
                            anchors.fill: parent
                            anchors.margins: 8
                            text: "⚠️ " + qsTr("Disabling metadata will prevent automatic mod updates and dependency resolution")
                            color: ThemeColors.warning
                            font.pixelSize: 12
                            wrapMode: Text.WordWrap
                        }
                    }

                    Rectangle { Layout.fillWidth: true; height: 1; color: Qt.rgba(ThemeColors.separator.r, ThemeColors.separator.g, ThemeColors.separator.b, 0.3) }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 12
                        
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2
                            
                            Label {
                                text: qsTr("Auto-install dependencies")
                                color: ThemeColors.text
                                font.pixelSize: 13
                            }
                            Label {
                                text: qsTr("Automatically download required libraries")
                                color: ThemeColors.textSecondary
                                font.pixelSize: 11
                            }
                        }
                        
                        Switch {
                            checked: vm ? vm.dependenciesEnabled : true
                            onCheckedChanged: if (vm) vm.dependenciesEnabled = checked
                        }
                    }
                }
            }

            // === Advanced ===
            SettingsSection {
                Layout.fillWidth: true
                title: qsTr("Advanced")
                iconSource: Theme.icon("settings")
                collapsible: true

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 16

                    GridLayout {
                        Layout.fillWidth: true
                        columns: 3
                        rowSpacing: 16
                        columnSpacing: 16

                        Label { text: qsTr("Max Concurrent Tasks"); color: ThemeColors.textSecondary; font.pixelSize: 13 }
                        Item { Layout.fillWidth: true }
                        SpinBox {
                            from: 1; to: 20
                            value: vm ? vm.concurrentTasks : 4
                            onValueModified: if (vm) vm.concurrentTasks = value
                        }

                        Label { text: qsTr("Max Concurrent Downloads"); color: ThemeColors.textSecondary; font.pixelSize: 13 }
                        Item { Layout.fillWidth: true }
                        SpinBox {
                            from: 1; to: 20
                            value: vm ? vm.concurrentDownloads : 6
                            onValueModified: if (vm) vm.concurrentDownloads = value
                        }
                        
                        Label { text: qsTr("Console Scrollback Limit"); color: ThemeColors.textSecondary; font.pixelSize: 13 }
                        Item { Layout.fillWidth: true }
                        SpinBox {
                            from: 10000; to: 1000000; stepSize: 10000
                            value: vm ? vm.logHistoryLimit : 100000
                            onValueModified: if (vm) vm.logHistoryLimit = value
                            
                            textFromValue: function(value) {
                                return value.toLocaleString();
                            }
                        }
                    }
                }
            }

            Item { height: 20 }
        }
    }

    // Browse functions
    function browseForInstancesFolder() { if (ProjT.launcherVM) { var r = ProjT.launcherVM.browseForFolder(qsTr("Select Instances Folder")); if (r && vm) vm.instancesFolder = r; } }
    function browseForModsFolder() { if (ProjT.launcherVM) { var r = ProjT.launcherVM.browseForFolder(qsTr("Select Mods Folder")); if (r && vm) vm.modsFolder = r; } }
    function browseForIconsFolder() { if (ProjT.launcherVM) { var r = ProjT.launcherVM.browseForFolder(qsTr("Select Icons Folder")); if (r && vm) vm.iconsFolder = r; } }
    function browseForJavaFolder() { if (ProjT.launcherVM) { var r = ProjT.launcherVM.browseForFolder(qsTr("Select Java Folder")); if (r && vm) vm.javaFolder = r; } }
    function browseForDownloadsFolder() { if (ProjT.launcherVM) { var r = ProjT.launcherVM.browseForFolder(qsTr("Select Downloads Folder")); if (r && vm) vm.downloadsFolder = r; } }
}
