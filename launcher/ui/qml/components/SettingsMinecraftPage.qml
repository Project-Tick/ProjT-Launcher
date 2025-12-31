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
    id: minecraftPage
    clip: true
    contentWidth: availableWidth
    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

    property var vm: ProjT.launcherSettingsVM

    Rectangle {
        width: minecraftPage.availableWidth
        implicitHeight: mainColumn.implicitHeight + 40
        color: "transparent"

        ColumnLayout {
            id: mainColumn
            width: Math.min(parent.width - 40, 700)
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 20
            spacing: 16

            // === Window Management ===
            SettingsSection {
                Layout.fillWidth: true
                title: qsTr("Window Manager")
                iconSource: Theme.icon("minecraft")

                ColumnLayout {
                    spacing: 16
                    Layout.fillWidth: true

                    ThemedCheckBox {
                        text: qsTr("Start Minecraft maximized")
                        description: qsTr("Game window will open in fullscreen mode")
                        checked: vm ? vm.startMaximized : false
                        onCheckedChanged: if (vm) vm.startMaximized = checked
                        useSwitch: true
                        Layout.fillWidth: true
                    }

                    Rectangle { Layout.fillWidth: true; height: 1; color: Qt.rgba(ThemeColors.separator.r, ThemeColors.separator.g, ThemeColors.separator.b, 0.3) }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 24

                        ColumnLayout {
                            spacing: 8
                            Label {
                                text: qsTr("Default Width")
                                color: ThemeColors.textSecondary
                                font.pixelSize: 12
                            }
                            SpinBox {
                                from: 256; to: 7680
                                value: vm ? vm.windowWidth : 854
                                editable: true
                                onValueChanged: if (vm) vm.windowWidth = value
                            }
                        }

                        ColumnLayout {
                            spacing: 8
                            Label {
                                text: qsTr("Default Height")
                                color: ThemeColors.textSecondary
                                font.pixelSize: 12
                            }
                            SpinBox {
                                from: 256; to: 4320
                                value: vm ? vm.windowHeight : 480
                                editable: true
                                onValueChanged: if (vm) vm.windowHeight = value
                            }
                        }
                        
                        Item { Layout.fillWidth: true }
                    }
                }
            }

            // === Game Time ===
            SettingsSection {
                Layout.fillWidth: true
                title: qsTr("Playtime Tracking")
                iconSource: Theme.icon("log")

                ColumnLayout {
                    spacing: 4
                    Layout.fillWidth: true

                    ThemedCheckBox {
                        text: qsTr("Record time spent playing")
                        description: qsTr("Track how much time you spend in each instance")
                        checked: vm ? vm.showGameTime : true
                        onCheckedChanged: if (vm) vm.showGameTime = checked
                        Layout.fillWidth: true
                    }
                    
                    ThemedCheckBox {
                        text: qsTr("Show playtime in instance list")
                        description: qsTr("Display total playtime on instance cards")
                        checked: vm ? vm.showGlobalGameTime : true
                        onCheckedChanged: if (vm) vm.showGlobalGameTime = checked
                        Layout.fillWidth: true
                    }
                }
            }

            // === System Integration ===
            SettingsSection {
                Layout.fillWidth: true
                title: qsTr("System Integration")
                iconSource: Theme.icon("externaltools")

                ColumnLayout {
                    spacing: 4
                    Layout.fillWidth: true
                    
                    ThemedCheckBox {
                        text: qsTr("Show game log in console")
                        description: qsTr("Open console window when game launches")
                        checked: vm ? vm.showGameLog : true
                        onCheckedChanged: if (vm) vm.showGameLog = checked
                        Layout.fillWidth: true
                    }
                    
                    ThemedCheckBox {
                        text: qsTr("Skip Mojang account migration check")
                        checked: vm ? vm.skipMigrationCheck : false
                        onCheckedChanged: if (vm) vm.skipMigrationCheck = checked
                        Layout.fillWidth: true
                    }
                    
                    Rectangle { Layout.fillWidth: true; height: 1; color: Qt.rgba(ThemeColors.separator.r, ThemeColors.separator.g, ThemeColors.separator.b, 0.3); Layout.topMargin: 8; Layout.bottomMargin: 8 }
                    
                    ThemedCheckBox {
                        text: qsTr("Use native OpenAL")
                        description: qsTr("Use system's audio library instead of bundled one")
                        checked: vm ? vm.useNativeOpenAL : false
                        onCheckedChanged: if (vm) vm.useNativeOpenAL = checked
                        Layout.fillWidth: true
                    }
                    
                    ThemedCheckBox {
                        text: qsTr("Use native GLFW")
                        description: qsTr("Use system's window library instead of bundled one")
                        checked: vm ? vm.useNativeGLFW : false
                        onCheckedChanged: if (vm) vm.useNativeGLFW = checked
                        Layout.fillWidth: true
                    }
                }
            }
            
            // === Modifications ===
            SettingsSection {
                Layout.fillWidth: true
                title: qsTr("Modifications")
                iconSource: Theme.icon("loadermods")
                
                ThemedCheckBox {
                    text: qsTr("Enable 'Manage Mods' button")
                    description: qsTr("Show mod management option in instance context menu")
                    checked: vm ? vm.enableManageModsButton : true
                    onCheckedChanged: if (vm) vm.enableManageModsButton = checked
                    Layout.fillWidth: true
                }
            }

            // === Linux Specific (Visible only on Linux) ===
            SettingsSection {
                Layout.fillWidth: true
                title: qsTr("Linux Optimizations")
                icon: "🐧"
                visible: Qt.platform.os === "linux"

                ColumnLayout {
                    spacing: 4
                    Layout.fillWidth: true
                    
                    ThemedCheckBox {
                        text: qsTr("Enable Feral GameMode")
                        description: qsTr("Apply CPU and GPU optimizations for gaming")
                        checked: vm ? vm.enableFeralGamemode : false
                        onCheckedChanged: if (vm) vm.enableFeralGamemode = checked
                        Layout.fillWidth: true
                    }
                    
                    ThemedCheckBox {
                        text: qsTr("Use discrete GPU (prime-run)")
                        description: qsTr("Force dedicated graphics on hybrid systems")
                        checked: vm ? vm.enableDiscreteGpu : false
                        onCheckedChanged: if (vm) vm.enableDiscreteGpu = checked
                        Layout.fillWidth: true
                    }
                    
                    ThemedCheckBox {
                        text: qsTr("Use MangoHud")
                        description: qsTr("Show performance overlay in game")
                        checked: vm ? vm.enableMangoHud : false
                        onCheckedChanged: if (vm) vm.enableMangoHud = checked
                        Layout.fillWidth: true
                    }
                }
            }
            
            Item { height: 20 }
        }
    }
}
