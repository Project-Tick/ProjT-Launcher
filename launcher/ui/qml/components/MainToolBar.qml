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
    id: toolbar
    height: 40

    // Theme binding - directly from themeVM for reliable updates
    property var themeVM: ProjT.themeVM
    property int _themeUpdateCount: 0

    // Compute toolbar color based on theme
    color: {
        var _ = _themeUpdateCount;
        return themeVM ? Qt.darker(themeVM.windowColor, 1.05) : ThemeColors.toolBar;
    }

    // Listen for theme changes
    Connections {
        target: themeVM
        function onThemeColorsChanged() {
            console.log("[MainToolBar] Theme colors changed");
            toolbar._themeUpdateCount++;
        }
    }

    // Signals (matching ShellRoot.qml usage)
    signal addInstance
    signal showSettings
    signal showAbout
    signal showLogs
    signal checkUpdate
    signal accountsMenuRequested

    // Folder actions
    signal openLauncherFolder
    signal openInstancesFolder
    signal openModsFolder
    signal openSkinsFolder

    // Computed colors for child elements
    property color toolBarColor: {
        var _ = _themeUpdateCount;
        return themeVM ? Qt.darker(themeVM.windowColor, 1.05) : ThemeColors.toolBar;
    }
    property color borderColor: {
        var _ = _themeUpdateCount;
        return themeVM ? Qt.darker(themeVM.windowColor, 1.2) : ThemeColors.border;
    }

    Rectangle {
        anchors.fill: parent
        color: toolbar.toolBarColor

        // Bottom border only
        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: 1
            color: toolbar.borderColor
        }

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: Theme.spacingS
            anchors.rightMargin: Theme.spacingS
            spacing: 2

            // === Add Instance Button ===
            ThemedToolButton {
                text: qsTr("Add Instance")
                icon.name: "list-add"
                display: AbstractButton.TextBesideIcon
                Layout.preferredHeight: 32

                onClicked: toolbar.addInstance()

                ToolTip.text: qsTr("Add a new instance")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }

            // Separator
            Rectangle {
                Layout.preferredWidth: 1
                Layout.preferredHeight: 20
                Layout.alignment: Qt.AlignVCenter
                color: toolbar.borderColor
            }

            // === Folders Menu ===
            ThemedToolButton {
                id: foldersBtn
                text: qsTr("Folders")
                icon.name: "folder"
                display: AbstractButton.TextBesideIcon
                Layout.preferredHeight: 32

                onClicked: foldersMenu.open()

                Menu {
                    id: foldersMenu
                    y: foldersBtn.height

                    background: Rectangle {
                        implicitWidth: 220
                        color: ThemeColors.surface
                        border.color: ThemeColors.border
                        radius: ThemeColors.radiusS
                    }

                    Action {
                        text: qsTr("View Launcher Root")
                        onTriggered: toolbar.openLauncherFolder()
                    }
                    MenuSeparator {
                        contentItem: Rectangle {
                            implicitHeight: 1
                            color: ThemeColors.border
                        }
                    }
                    Action {
                        text: qsTr("View Instance Folder")
                        onTriggered: toolbar.openInstancesFolder()
                    }
                    Action {
                        text: qsTr("View Central Mods Folder")
                        onTriggered: toolbar.openModsFolder()
                    }
                    Action {
                        text: qsTr("View Skins Folder")
                        onTriggered: toolbar.openSkinsFolder()
                    }

                    delegate: MenuItem {
                        id: menuItem
                        implicitHeight: 32

                        contentItem: Text {
                            text: menuItem.text
                            font.pixelSize: 12
                            color: menuItem.highlighted ? ThemeColors.highlightedText : ThemeColors.text
                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                            elide: Text.ElideRight
                        }

                        background: Rectangle {
                            color: menuItem.highlighted ? ThemeColors.highlight : "transparent"
                            radius: ThemeColors.radiusS - 2
                        }
                    }
                }
            }

            // === Settings Button ===
            ThemedToolButton {
                text: qsTr("Settings")
                icon.name: "configure"
                display: AbstractButton.TextBesideIcon
                Layout.preferredHeight: 32

                onClicked: toolbar.showSettings()

                ToolTip.text: qsTr("Change settings")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }

            // === Help Menu ===
            ThemedToolButton {
                id: helpBtn
                text: qsTr("Help")
                icon.name: "help-about"
                display: AbstractButton.TextBesideIcon
                Layout.preferredHeight: 32

                onClicked: helpMenu.open()

                Menu {
                    id: helpMenu
                    y: helpBtn.height

                    background: Rectangle {
                        implicitWidth: 180
                        color: ThemeColors.surface
                        border.color: ThemeColors.border
                        radius: ThemeColors.radiusS
                    }

                    Action {
                        text: qsTr("About")
                        onTriggered: toolbar.showAbout()
                    }
                    MenuSeparator {
                        contentItem: Rectangle {
                            implicitHeight: 1
                            color: ThemeColors.border
                        }
                    }
                    Action {
                        text: qsTr("View Logs")
                        onTriggered: toolbar.showLogs()
                    }

                    delegate: MenuItem {
                        id: helpMenuItem
                        implicitHeight: 32

                        contentItem: Text {
                            text: helpMenuItem.text
                            font.pixelSize: 12
                            color: helpMenuItem.highlighted ? ThemeColors.highlightedText : ThemeColors.text
                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                            elide: Text.ElideRight
                        }

                        background: Rectangle {
                            color: helpMenuItem.highlighted ? ThemeColors.highlight : "transparent"
                            radius: ThemeColors.radiusS - 2
                        }
                    }
                }
            }

            // === Check Update Button ===
            ThemedToolButton {
                text: qsTr("Update")
                icon.name: "update-none"
                display: AbstractButton.TextBesideIcon
                Layout.preferredHeight: 32
                visible: ProjT.launcherSettingsVM ? ProjT.launcherSettingsVM.checkForUpdates : true

                onClicked: toolbar.checkUpdate()

                ToolTip.text: qsTr("Check for updates")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }

            // === Spacer ===
            Item {
                Layout.fillWidth: true
            }

            // Separator
            Rectangle {
                Layout.preferredWidth: 1
                Layout.preferredHeight: 20
                Layout.alignment: Qt.AlignVCenter
                color: ThemeColors.border
            }

            // === Accounts Menu ===
            ThemedToolButton {
                id: accountsBtn
                text: ProjT.accountsVM && ProjT.accountsVM.defaultAccountName ? ProjT.accountsVM.defaultAccountName : qsTr("Accounts")
                icon.name: "user"
                display: AbstractButton.TextBesideIcon
                Layout.preferredHeight: 32

                onClicked: toolbar.accountsMenuRequested()

                ToolTip.text: qsTr("Open account settings")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }
        }
    }
}
