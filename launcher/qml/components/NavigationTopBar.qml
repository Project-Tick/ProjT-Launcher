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
    id: topBar
    color: ThemeColors.toolBar
    height: 48
    
    // Signals
    signal pageRequested(int page)
    signal createNewInstance()
    signal accountsMenuRequested()
    
    // Navigation entries
    readonly property var navEntries: [
        { title: qsTr("Instances"), page: LauncherViewModelEnums.Page.Instances },
        { title: qsTr("News"), page: LauncherViewModelEnums.Page.News },
        { title: qsTr("About"), page: LauncherViewModelEnums.Page.About },
        { title: qsTr("Settings"), page: LauncherViewModelEnums.Page.Settings }
    ]
    
    Rectangle {
        anchors.fill: parent
        color: ThemeColors.toolBar
        
        // Bottom border only
        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: 1
            color: ThemeColors.border
        }
        
        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: Theme.spacingM
            anchors.rightMargin: Theme.spacingM
            spacing: 0
            
            // === Logo / App Name ===
            RowLayout {
                spacing: Theme.spacingS
                Layout.rightMargin: Theme.spacingM
                
                Label {
                    text: qsTr("ProjT")
                    color: ThemeColors.text
                    font.pointSize: 14
                    font.bold: true
                }
            }
            
            // Separator
            Rectangle {
                Layout.preferredWidth: 1
                Layout.preferredHeight: 28
                Layout.alignment: Qt.AlignVCenter
                color: ThemeColors.border
            }
            
            // === Page Navigation Buttons ===
            Repeater {
                model: navEntries
                
                delegate: ThemedToolButton {
                    id: navBtn
                    text: modelData.title
                    checkable: true
                    checked: ProjT.launcherVM && ProjT.launcherVM.currentPage === modelData.page
                    active: checked
                    Layout.preferredHeight: 36
                    Layout.leftMargin: index === 0 ? Theme.spacingS : 0
                    
                    onClicked: {
                        if (ProjT.launcherVM) {
                            ProjT.launcherVM.currentPage = modelData.page
                        }
                        topBar.pageRequested(modelData.page)
                    }
                }
            }
            
            // === Spacer ===
            Item { Layout.fillWidth: true }
            
            // Separator
            Rectangle {
                Layout.preferredWidth: 1
                Layout.preferredHeight: 28
                Layout.alignment: Qt.AlignVCenter
                color: ThemeColors.border
            }
            
            // === Add Instance Button ===
            ThemedButton {
                text: qsTr("+ New")
                success: true
                size: "small"
                Layout.preferredHeight: 32
                
                onClicked: topBar.createNewInstance()
                
                ToolTip.text: qsTr("Create or import a new instance")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }
            
            // === Accounts Button ===
            ThemedToolButton {
                id: accountsBtn
                text: ProjT.accountsVM && ProjT.accountsVM.defaultAccountName 
                      ? "👤 " + ProjT.accountsVM.defaultAccountName.substring(0, 10) 
                      : "👤"
                Layout.preferredHeight: 32
                
                onClicked: topBar.accountsMenuRequested()
                
                ToolTip.text: qsTr("Open account settings")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }
        }
    }
}
