// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * TopBar Component - Main Application Toolbar
 * 
 * Equivalent to MainWindow's mainToolBar in Widgets version
 * Contains: Add Instance, Folders, Settings, Help, Updates, CAT, Accounts
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "." as Components
import "../Theme.js" as Theme

Rectangle {
    id: topBar
    color: Theme.surface
    height: 48
    width: parent.width
    
    // Signals
    signal createNewInstance()
    signal openFolders()
    signal openSettings()
    signal openHelp()
    signal checkUpdates()
    signal catAction()
    signal accountsMenu()
    
    Rectangle {
        anchors.fill: parent
        color: Theme.surface
        border.color: "#323742"
        border.width: 1
        
        RowLayout {
            anchors.fill: parent
            anchors.margins: 0
            spacing: Theme.spacingM
            anchors.leftMargin: Theme.spacingM
            anchors.rightMargin: Theme.spacingM
            
            // === Add Instance ===
            Button {
                text: qsTr("Add Instance")
                icon.name: "document-new"
                Layout.preferredHeight: 32
                
                onClicked: topBar.createNewInstance()
                
                ToolTip.text: qsTr("Create or import a new instance")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }
            
            ToolSeparator {
                orientation: Qt.Vertical
                Layout.fillHeight: true
            }
            
            // === Folders ===
            Button {
                text: qsTr("Folders")
                icon.name: "folder"
                Layout.preferredHeight: 32
                
                onClicked: topBar.openFolders()
                
                ToolTip.text: qsTr("Open folders")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }
            
            // === Settings ===
            Button {
                text: qsTr("Settings")
                icon.name: "preferences-system"
                Layout.preferredHeight: 32
                
                onClicked: topBar.openSettings()
                
                ToolTip.text: qsTr("Application settings")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }
            
            // === Help ===
            Button {
                text: qsTr("Help")
                icon.name: "help-browser"
                Layout.preferredHeight: 32
                
                onClicked: topBar.openHelp()
                
                ToolTip.text: qsTr("Open help")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }
            
            // === Check Updates ===
            Button {
                text: qsTr("Updates")
                icon.name: "system-software-update"
                Layout.preferredHeight: 32
                
                onClicked: topBar.checkUpdates()
                
                ToolTip.text: qsTr("Check for updates")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }
            
            ToolSeparator {
                orientation: Qt.Vertical
                Layout.fillHeight: true
            }
            
            // === CAT ===
            Button {
                text: "🐱"
                Layout.preferredWidth: 40
                Layout.preferredHeight: 32
                
                onClicked: topBar.catAction()
                
                ToolTip.text: qsTr("CAT")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }
            
            // === Accounts ===
            Button {
                id: accountsButton
                text: qsTr("Accounts")
                icon.name: "contact-new"
                Layout.preferredHeight: 32
                
                onClicked: accountsMenu.open()
                
                ToolTip.text: qsTr("Manage accounts")
                ToolTip.visible: hovered
                ToolTip.delay: 500
                
                Menu {
                    id: accountsMenu
                    y: accountsButton.height
                    
                    MenuItem {
                        text: qsTr("Manage Accounts")
                        onTriggered: topBar.accountsMenu()
                    }
                    
                    MenuSeparator {}
                    
                    MenuItem {
                        text: qsTr("No Accounts")
                        enabled: false
                    }
                }
            }
            
            // Spacer
            Item {
                Layout.fillWidth: true
            }
        }
    }
}
