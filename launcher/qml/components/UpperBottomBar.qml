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
import "../Theme.js" as Theme

Rectangle {
    id: upperBar
    color: Theme.surface
    height: 36
    
    // Properties - bound to ViewModels with safe defaults
    property string newsHeadline: ProjT.newsVM.latestHeadline || qsTr("Welcome to ProjT Launcher")
    property bool hasUpdate: ProjT.launcherVM.hasUpdate || false
    property string updateVersion: ProjT.launcherVM.updateVersion || ""
    
    // Signals
    signal moreNewsClicked()
    signal updateClicked()
    
    Rectangle {
        anchors.fill: parent
        color: Theme.surface
        border.color: "#323742"
        border.width: 1
        
        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: Theme.spacingM
            anchors.rightMargin: Theme.spacingM
            spacing: Theme.spacingM
            
            // === News Label ===
            Label {
                text: qsTr("News:")
                color: Theme.textSecondary
                font.pointSize: 10
                Layout.alignment: Qt.AlignVCenter
            }
            
            // === News Headline ===
            Label {
                id: headlineLabel
                text: newsHeadline
                color: Theme.textPrimary
                font.pointSize: 10
                elide: Text.ElideRight
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter
                
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: upperBar.moreNewsClicked()
                    
                    hoverEnabled: true
                    onEntered: headlineLabel.color = Theme.accent
                    onExited: headlineLabel.color = Theme.textPrimary
                }
            }
            
            // === More News Button ===
            Button {
                text: qsTr("More News")
                Layout.preferredHeight: 28
                Layout.alignment: Qt.AlignVCenter
                flat: true
                
                contentItem: Text {
                    text: parent.text
                    color: Theme.accent
                    font.pointSize: 9
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                
                background: Rectangle {
                    radius: 4
                    color: parent.hovered ? "#2a2d33" : "transparent"
                }
                
                onClicked: upperBar.moreNewsClicked()
            }
            
            // === Separator (visible only when update is available) ===
            ToolSeparator {
                visible: hasUpdate
                orientation: Qt.Vertical
                Layout.fillHeight: true
            }
            
            // === Update Available Badge ===
            Rectangle {
                visible: hasUpdate
                Layout.preferredWidth: updateRow.implicitWidth + 16
                Layout.preferredHeight: 28
                Layout.alignment: Qt.AlignVCenter
                radius: 14
                color: "#1e3a5f"
                border.color: "#3b82f6"
                border.width: 1
                
                RowLayout {
                    id: updateRow
                    anchors.centerIn: parent
                    spacing: 4
                    
                    Text {
                        text: "⬆"
                        font.pointSize: 10
                        color: "#3b82f6"
                    }
                    
                    Label {
                        text: qsTr("Update: %1").arg(updateVersion)
                        color: "#93c5fd"
                        font.pointSize: 9
                    }
                }
                
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: upperBar.updateClicked()
                }
            }
        }
    }
}
