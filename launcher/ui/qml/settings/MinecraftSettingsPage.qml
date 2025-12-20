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
    id: root
    objectName: "minecraftSettings"
    color: ThemeColors.background

    property var vm: ProjT.instanceVM

    Flickable {
        anchors.fill: parent
        anchors.margins: Theme.spacingM
        contentHeight: column.height
        clip: true

        ColumnLayout {
            id: column
            width: parent.width
            spacing: Theme.spacingM

            // Header
            Text {
                text: qsTr("Minecraft Settings")
                font.pixelSize: 24
                font.weight: Font.Bold
                color: ThemeColors.text
            }

            Text {
                text: root.vm && root.vm.instanceName ? root.vm.instanceName : qsTr("No instance selected")
                font.pixelSize: 14
                color: ThemeColors.textSecondary
                visible: root.vm !== null
            }

            // Window Settings
            GroupBox {
                title: qsTr("Window")
                Layout.fillWidth: true

                background: Rectangle {
                    color: ThemeColors.disabled
                    border.color: ThemeColors.hover
                    border.width: 1
                    radius: Theme.radiusS
                }

                label: Text {
                    text: parent.title
                    color: ThemeColors.text
                    font.weight: Font.DemiBold
                    leftPadding: Theme.spacingS
                }

                GridLayout {
                    width: parent.width
                    columns: 2
                    columnSpacing: Theme.spacingM
                    rowSpacing: Theme.spacingS

                    // Override checkbox
                    Text {
                        text: ""
                        color: ThemeColors.text
                    }
                    CheckBox {
                        id: overrideWindowCheck
                        text: qsTr("Override window settings")
                        checked: root.vm ? root.vm.overrideWindow : false
                        onCheckedChanged: if (root.vm)
                            root.vm.overrideWindow = checked

                        contentItem: Text {
                            text: overrideWindowCheck.text
                            color: ThemeColors.text
                            leftPadding: overrideWindowCheck.indicator.width + 8
                            verticalAlignment: Text.AlignVCenter
                        }
                    }

                    Text {
                        text: qsTr("Width:")
                        color: overrideWindowCheck.checked ? ThemeColors.text : ThemeColors.textSecondary
                    }
                    SpinBox {
                        id: widthSpinBox
                        from: 320
                        to: 7680
                        value: root.vm ? root.vm.windowWidth : 1280
                        enabled: overrideWindowCheck.checked
                        Layout.fillWidth: true
                        onValueModified: if (root.vm)
                            root.vm.windowWidth = value

                        background: Rectangle {
                            color: ThemeColors.surface
                            border.color: widthSpinBox.enabled ? ThemeColors.accent : ThemeColors.hover
                            border.width: 1
                            radius: Theme.radiusS
                            opacity: widthSpinBox.enabled ? 1.0 : 0.5
                        }
                    }

                    Text {
                        text: qsTr("Height:")
                        color: overrideWindowCheck.checked ? ThemeColors.text : ThemeColors.textSecondary
                    }
                    SpinBox {
                        id: heightSpinBox
                        from: 240
                        to: 4320
                        value: root.vm ? root.vm.windowHeight : 720
                        enabled: overrideWindowCheck.checked
                        Layout.fillWidth: true
                        onValueModified: if (root.vm)
                            root.vm.windowHeight = value

                        background: Rectangle {
                            color: ThemeColors.surface
                            border.color: heightSpinBox.enabled ? ThemeColors.accent : ThemeColors.hover
                            border.width: 1
                            radius: Theme.radiusS
                            opacity: heightSpinBox.enabled ? 1.0 : 0.5
                        }
                    }

                    Text {
                        text: ""
                        color: ThemeColors.text
                    }
                    CheckBox {
                        id: fullscreenCheck
                        text: qsTr("Fullscreen")
                        enabled: overrideWindowCheck.checked
                        checked: root.vm ? root.vm.fullscreen : false
                        onCheckedChanged: if (root.vm)
                            root.vm.fullscreen = checked

                        contentItem: Text {
                            text: fullscreenCheck.text
                            color: fullscreenCheck.enabled ? ThemeColors.text : ThemeColors.textSecondary
                            leftPadding: fullscreenCheck.indicator.width + 8
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                }
            }

            // Game Options
            GroupBox {
                title: qsTr("Game Options")
                Layout.fillWidth: true

                background: Rectangle {
                    color: ThemeColors.disabled
                    border.color: ThemeColors.hover
                    border.width: 1
                    radius: Theme.radiusS
                }

                label: Text {
                    text: parent.title
                    color: ThemeColors.text
                    font.weight: Font.DemiBold
                    leftPadding: Theme.spacingS
                }

                ColumnLayout {
                    width: parent.width
                    spacing: Theme.spacingS

                    CheckBox {
                        id: showConsoleCheck
                        text: qsTr("Show console while game is running")
                        checked: root.vm ? root.vm.showConsole : true
                        onCheckedChanged: if (root.vm)
                            root.vm.showConsole = checked

                        contentItem: Text {
                            text: showConsoleCheck.text
                            color: ThemeColors.text
                            leftPadding: showConsoleCheck.indicator.width + 8
                            verticalAlignment: Text.AlignVCenter
                        }
                    }

                    CheckBox {
                        id: closeOnLaunchCheck
                        text: qsTr("Close launcher when game starts")
                        checked: root.vm ? root.vm.closeOnLaunch : false
                        onCheckedChanged: if (root.vm)
                            root.vm.closeOnLaunch = checked

                        contentItem: Text {
                            text: closeOnLaunchCheck.text
                            color: ThemeColors.text
                            leftPadding: closeOnLaunchCheck.indicator.width + 8
                            verticalAlignment: Text.AlignVCenter
                        }
                    }

                    CheckBox {
                        id: quitAfterGameCheck
                        text: qsTr("Quit launcher after game exits")
                        checked: root.vm ? root.vm.quitAfterGame : false
                        onCheckedChanged: if (root.vm)
                            root.vm.quitAfterGame = checked

                        contentItem: Text {
                            text: quitAfterGameCheck.text
                            color: ThemeColors.text
                            leftPadding: quitAfterGameCheck.indicator.width + 8
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                }
            }

            // Actions
            RowLayout {
                Layout.fillWidth: true
                Layout.topMargin: Theme.spacingM
                spacing: Theme.spacingM

                Item {
                    Layout.fillWidth: true
                }

                Button {
                    text: qsTr("Reset")
                    onClicked: if (root.vm)
                        root.vm.reloadSettings()

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
                    text: qsTr("Save")
                    onClicked: if (root.vm)
                        root.vm.saveSettings()

                    background: Rectangle {
                        color: parent.hovered ? Qt.lighter(ThemeColors.accent, 1.1) : ThemeColors.accent
                        radius: Theme.radiusS
                    }

                    contentItem: Text {
                        text: parent.text
                        color: ThemeColors.surface
                        font.weight: Font.DemiBold
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }

            Item {
                Layout.fillHeight: true
            }
        }
    }
}
