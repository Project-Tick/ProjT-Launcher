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
    color: Theme.surface
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
        color: Theme.surface
        border.color: "#323742"
        border.width: 1
        
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
                    color: Theme.textPrimary
                    font.pointSize: 14
                    font.bold: true
                }
            }
            
            ToolSeparator {
                orientation: Qt.Vertical
                Layout.fillHeight: true
            }
            
            // === Page Navigation Buttons ===
            Repeater {
                model: navEntries
                
                delegate: Button {
                    id: navBtn
                    text: modelData.title
                    checkable: true
                    checked: ProjT.launcherVM && ProjT.launcherVM.currentPage === modelData.page
                    Layout.preferredHeight: 36
                    Layout.leftMargin: index === 0 ? Theme.spacingS : 0
                    
                    background: Rectangle {
                        radius: Theme.radius
                        color: navBtn.checked ? "#2c3440" : (navBtn.hovered ? "#2a2d33" : "transparent")
                        border.color: navBtn.checked ? Theme.accent : "transparent"
                        border.width: navBtn.checked ? 1 : 0
                        
                        Behavior on color { ColorAnimation { duration: 100 } }
                    }
                    
                    contentItem: RowLayout {
                        spacing: 4
                        
                        Text {
                            text: modelData.icon
                            font.pointSize: 11
                            visible: navBtn.checked || navBtn.hovered
                        }
                        
                        Text {
                            text: navBtn.text
                            color: navBtn.checked ? "#e6f0ff" : Theme.textPrimary
                            font.pointSize: 11
                            font.bold: navBtn.checked
                        }
                    }
                    
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
            
            ToolSeparator {
                orientation: Qt.Vertical
                Layout.fillHeight: true
            }
            
            // === Add Instance Button ===
            Button {
                text: qsTr("+ New")
                Layout.preferredHeight: 32
                
                background: Rectangle {
                    radius: Theme.radius
                    color: parent.hovered ? "#166534" : "#15803d"
                    border.color: "#22c55e"
                    border.width: 1
                }
                
                contentItem: Text {
                    text: parent.text
                    color: "#ffffff"
                    font.pointSize: 10
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                
                onClicked: topBar.createNewInstance()
                
                ToolTip.text: qsTr("Create or import a new instance")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }
            
            // === Accounts Button ===
            Button {
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
