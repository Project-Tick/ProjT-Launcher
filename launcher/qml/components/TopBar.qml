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
import "." as Components
import "../Theme.js" as Theme

Rectangle {
    id: topBar
    color: ThemeColors.surface
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
        color: ThemeColors.surface
        border.color: ThemeColors.border
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
                text: ProjT.accountsVM && ProjT.accountsVM.defaultAccountName 
                      ? ProjT.accountsVM.defaultAccountName 
                      : qsTr("Accounts")
                icon.name: "contact-new"
                Layout.preferredHeight: 32
                
                onClicked: topBar.accountsMenu()
                
                ToolTip.text: qsTr("Open account settings")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }
            
            // Spacer
            Item {
                Layout.fillWidth: true
            }
        }
    }
}
