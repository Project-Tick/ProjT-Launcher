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
import QtQml 2.15
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
    property color toolBarColor: ThemeColors.background
    property color borderColor: ThemeColors.separator

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
            opacity: 0.8
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

                onClicked: accountsMenu.open()

                Menu {
                    id: accountsMenu
                    y: accountsBtn.height

                    background: Rectangle {
                        implicitWidth: 240
                        color: ThemeColors.surface
                        border.color: ThemeColors.border
                        radius: ThemeColors.radiusS
                    }

                    // Account List
                    Instantiator {
                        model: ProjT.accountsVM ? ProjT.accountsVM.model : null
                        delegate: MenuItem {
                            id: accountMenuItem
                            property int modelIndex: index
                            property bool isDefault: ProjT.accountsVM && ProjT.accountsVM.isAccountDefault(index)
                            
                            implicitHeight: 36
                            
                            contentItem: RowLayout {
                                spacing: Theme.spacingS
                                
                                // Avatar placeholder (could be replaced with actual skin head if available)
                                Rectangle {
                                    width: 20
                                    height: 20
                                    color: ThemeColors.backgroundAlt
                                    radius: 2
                                    Image {
                                        anchors.fill: parent
                                        source: "qrc:/icons/multimc/scalable/status/steve.svg" // Fallback
                                        fillMode: Image.PreserveAspectFit
                                        visible: true // In future binding to skin
                                    }
                                }

                                Label {
                                    text: ProjT.accountsVM ? ProjT.accountsVM.getAccountName(modelIndex) : ""
                                    color: ThemeColors.text
                                    font.bold: isDefault
                                    Layout.fillWidth: true
                                    elide: Text.ElideRight
                                    verticalAlignment: Text.AlignVCenter
                                }

                                Label {
                                    text: "✓"
                                    visible: isDefault
                                    color: ThemeColors.accent
                                    font.bold: true
                                }
                            }
                            
                            background: Rectangle {
                                color: accountMenuItem.highlighted ? ThemeColors.highlight : "transparent"
                                radius: ThemeColors.radiusS - 2
                            }

                            onTriggered: {
                                if (ProjT.accountsVM) {
                                    ProjT.accountsVM.setDefaultAccount(modelIndex);
                                }
                            }
                        }
                        onObjectAdded: accountsMenu.insertItem(index, object)
                        onObjectRemoved: accountsMenu.removeItem(object)
                    }

                    MenuSeparator {
                        visible: ProjT.accountsVM && ProjT.accountsVM.hasAccounts
                        contentItem: Rectangle {
                            implicitHeight: 1
                            color: ThemeColors.border
                        }
                    }

                    MenuItem {
                        text: qsTr("Manage Accounts...")
                        icon.name: "configure"
                        
                        contentItem: RowLayout {
                            spacing: Theme.spacingS
                            Image {
                                source: "qrc:/icons/multimc/scalable/settings.svg" // Fallback icon path or theme icon
                                Layout.preferredWidth: 16
                                Layout.preferredHeight: 16
                                visible: false // Use text only for consistency if icons missing
                            }
                            Label {
                                text: parent.parent.text
                                color: ThemeColors.text
                                verticalAlignment: Text.AlignVCenter
                            }
                        }
                        
                        background: Rectangle {
                            color: parent.highlighted ? ThemeColors.highlight : "transparent"
                            radius: ThemeColors.radiusS - 2
                        }
                        
                        onTriggered: toolbar.accountsMenuRequested()
                    }
                    
                    MenuItem {
                        text: qsTr("Add Microsoft Account")
                        
                        contentItem: Label {
                            text: parent.text
                            color: ThemeColors.text
                            verticalAlignment: Text.AlignVCenter
                        }
                        
                        background: Rectangle {
                            color: parent.highlighted ? ThemeColors.highlight : "transparent"
                            radius: ThemeColors.radiusS - 2
                        }
                        
                        onTriggered: {
                             if (ProjT.accountsVM) ProjT.accountsVM.addMicrosoftAccount();
                             toolbar.accountsMenuRequested(); // Go to accounts page to see login dialog
                        }
                    }
                }
            }
        }
    }
}
