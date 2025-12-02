// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  MainToolBar - Primary application toolbar
 *  
 *  Matches Widget MainWindow mainToolBar layout:
 *  [Add Instance] | [Folders v] [Settings] [Help v] [Update] | [Accounts v]
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme.js" as Theme

Rectangle {
    id: toolbar
    color: Theme.surface
    height: 40
    
    // Signals (matching ShellRoot.qml usage)
    signal addInstance()
    signal showSettings()
    signal showAbout()
    signal showLogs()
    signal checkUpdate()
    signal accountsMenuRequested()
    
    // Folder actions
    signal openLauncherFolder()
    signal openInstancesFolder()
    signal openModsFolder()
    signal openSkinsFolder()
    
    Rectangle {
        anchors.fill: parent
        color: Theme.surface
        border.color: "#323742"
        border.width: 1
        
        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: Theme.spacingS
            anchors.rightMargin: Theme.spacingS
            spacing: 2
            
            // === Add Instance Button ===
            ToolButton {
                text: qsTr("Add Instance")
                icon.name: "list-add"
                display: AbstractButton.TextBesideIcon
                Layout.preferredHeight: 32
                
                onClicked: toolbar.addInstance()
                
                ToolTip.text: qsTr("Add a new instance")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }
            
            ToolSeparator {
                Layout.fillHeight: true
            }
            
            // === Folders Menu ===
            ToolButton {
                id: foldersBtn
                text: qsTr("Folders")
                icon.name: "folder"
                display: AbstractButton.TextBesideIcon
                Layout.preferredHeight: 32
                
                onClicked: foldersMenu.open()
                
                Menu {
                    id: foldersMenu
                    y: foldersBtn.height
                    
                    Action {
                        text: qsTr("View Launcher Root")
                        onTriggered: toolbar.openLauncherFolder()
                    }
                    MenuSeparator {}
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
                }
            }
            
            // === Settings Button ===
            ToolButton {
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
            ToolButton {
                id: helpBtn
                text: qsTr("Help")
                icon.name: "help-about"
                display: AbstractButton.TextBesideIcon
                Layout.preferredHeight: 32
                
                onClicked: helpMenu.open()
                
                Menu {
                    id: helpMenu
                    y: helpBtn.height
                    
                    Action {
                        text: qsTr("About")
                        onTriggered: toolbar.showAbout()
                    }
                    MenuSeparator {}
                    Action {
                        text: qsTr("View Logs")
                        onTriggered: toolbar.showLogs()
                    }
                }
            }
            
            // === Check Update Button ===
            ToolButton {
                text: qsTr("Update")
                icon.name: "update-none"
                display: AbstractButton.TextBesideIcon
                Layout.preferredHeight: 32
                visible: true  // TODO: Check if updates are enabled
                
                onClicked: toolbar.checkUpdate()
                
                ToolTip.text: qsTr("Check for updates")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }
            
            // === Spacer ===
            Item { Layout.fillWidth: true }
            
            ToolSeparator {
                Layout.fillHeight: true
            }
            
            // === Accounts Menu ===
            ToolButton {
                id: accountsBtn
                text: qsTr("Accounts")
                icon.name: "user"
                display: AbstractButton.TextBesideIcon
                Layout.preferredHeight: 32
                
                onClicked: accountsMenu.open()
                
                Menu {
                    id: accountsMenu
                    y: accountsBtn.height
                    
                    Action {
                        text: qsTr("Manage Accounts...")
                        onTriggered: toolbar.accountsMenuRequested()
                    }
                    MenuSeparator {}
                    MenuItem {
                        text: qsTr("No accounts")
                        enabled: false
                    }
                }
            }
        }
    }
}
