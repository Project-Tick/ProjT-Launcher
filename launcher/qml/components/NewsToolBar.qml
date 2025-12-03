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
    id: newsToolbar
    color: Theme.surface
    height: 32
    
    // News data from ViewModel
    readonly property var newsVM: ProjT.newsVM
    readonly property string latestHeadline: newsVM ? newsVM.latestHeadline : ""
    readonly property bool isBusy: newsVM ? newsVM.busy : false
    
    // Signals
    signal newsClicked()
    signal moreNewsClicked()
    
    Rectangle {
        anchors.fill: parent
        color: Theme.surface
        border.color: "#323742"
        border.width: 1
        
        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: Theme.spacingS
            anchors.rightMargin: Theme.spacingS
            spacing: Theme.spacingS
            
            // === News Icon ===
            Label {
                text: "📰"
                font.pointSize: 12
                Layout.alignment: Qt.AlignVCenter
            }
            
            // News label
            Label {
                text: qsTr("News:")
                color: Theme.textSecondary
                font.pointSize: 10
                Layout.alignment: Qt.AlignVCenter
            }
            
            // === News Headline (clickable) ===
            Label {
                id: headlineLabel
                text: isBusy ? qsTr("Loading news...") : (latestHeadline.length > 0 ? latestHeadline : qsTr("No news available"))
                color: Theme.textPrimary
                font.pointSize: 10
                elide: Text.ElideRight
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter
                
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    hoverEnabled: true
                    
                    onClicked: newsToolbar.newsClicked()
                    onEntered: headlineLabel.color = Theme.accent
                    onExited: headlineLabel.color = Theme.textPrimary
                }
            }
            
            // === More News Button ===
            ToolButton {
                text: qsTr("More News...")
                icon.name: "go-next"
                display: AbstractButton.TextBesideIcon
                Layout.preferredHeight: 26
                
                onClicked: newsToolbar.moreNewsClicked()
                
                ToolTip.text: qsTr("Open the development blog to read more news")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }
        }
    }
}
