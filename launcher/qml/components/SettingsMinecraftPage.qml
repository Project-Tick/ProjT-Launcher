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

ScrollView {
    id: minecraftPage
    clip: true

    property var vm: ProjT.launcherSettingsVM

    ColumnLayout {
        width: minecraftPage.width - Theme.spacingL
        spacing: Theme.spacingM

        // Window
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Window")

            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS

                CheckBox {
                    id: maximizedCheck
                    text: qsTr("Start Minecraft maximized")
                    checked: vm ? vm.startMaximized : false
                    onCheckedChanged: if (vm)
                        vm.startMaximized = checked
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingM

                    Label {
                        text: qsTr("Window width:")
                        color: ThemeColors.text
                    }

                    SpinBox {
                        id: windowWidthSpin
                        from: 256
                        to: 4096
                        value: vm ? vm.windowWidth : 854
                        editable: true
                        onValueChanged: if (vm)
                            vm.windowWidth = value
                    }

                    Label {
                        text: qsTr("Window height:")
                        color: ThemeColors.text
                    }

                    SpinBox {
                        id: windowHeightSpin
                        from: 256
                        to: 4096
                        value: vm ? vm.windowHeight : 480
                        editable: true
                        onValueChanged: if (vm)
                            vm.windowHeight = value
                    }
                }
            }
        }

        // Game Time
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Game Time")

            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS

                CheckBox {
                    text: qsTr("Record time spent playing")
                    checked: vm ? vm.showGameTime : true
                    onCheckedChanged: if (vm)
                        vm.showGameTime = checked
                }

                CheckBox {
                    text: qsTr("Show time spent playing in the instance list")
                    checked: vm ? vm.showGlobalGameTime : true
                    onCheckedChanged: if (vm)
                        vm.showGlobalGameTime = checked
                }
            }
        }

        // Mods Management
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Mods Management")

            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS

                CheckBox {
                    text: qsTr("Enable \"Manage Mods\" button")
                    checked: vm ? vm.enableManageModsButton : true
                    onCheckedChanged: if (vm)
                        vm.enableManageModsButton = checked
                }
            }
        }

        // Linux Specific
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Linux Specific")
            visible: Qt.platform.os === "linux"

            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS

                CheckBox {
                    text: qsTr("Enable Feral GameMode")
                    checked: vm ? vm.enableFeralGamemode : false
                    onCheckedChanged: if (vm)
                        vm.enableFeralGamemode = checked
                }

                CheckBox {
                    text: qsTr("Use discrete GPU (prime-run)")
                    checked: vm ? vm.enableDiscreteGpu : false
                    onCheckedChanged: if (vm)
                        vm.enableDiscreteGpu = checked
                }

                CheckBox {
                    text: qsTr("Use MangoHud")
                    checked: vm ? vm.enableMangoHud : false
                    onCheckedChanged: if (vm)
                        vm.enableMangoHud = checked
                }
            }
        }

        // Miscellaneous
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Miscellaneous")

            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS

                CheckBox {
                    text: qsTr("Show game log in console")
                    checked: vm ? vm.showGameLog : true
                    onCheckedChanged: if (vm)
                        vm.showGameLog = checked
                }

                CheckBox {
                    text: qsTr("Skip Mojang account migration check")
                    checked: vm ? vm.skipMigrationCheck : false
                    onCheckedChanged: if (vm)
                        vm.skipMigrationCheck = checked
                }

                CheckBox {
                    text: qsTr("Use native OpenAL")
                    checked: vm ? vm.useNativeOpenAL : false
                    onCheckedChanged: if (vm)
                        vm.useNativeOpenAL = checked
                }

                CheckBox {
                    text: qsTr("Use native GLFW")
                    checked: vm ? vm.useNativeGLFW : false
                    onCheckedChanged: if (vm)
                        vm.useNativeGLFW = checked
                }
            }
        }

        Item {
            height: Theme.spacingL
        }
    }
}
