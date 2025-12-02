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
import "." as Components
import "../Theme.js" as Theme

Rectangle {
    id: bottomBar
    color: Theme.surface
    height: 40
    width: parent.width
    
    // Properties
    property string statusMessage: qsTr("Ready")
    
    // Signals
    signal moreNewsRequested()
    
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
            
            // === Status Message ===
            Label {
                text: bottomBar.statusMessage
                color: Theme.textSecondary
                font.pointSize: 9
                Layout.fillWidth: true
                
                elide: Text.ElideRight
            }
            
            ToolSeparator {
                orientation: Qt.Vertical
                Layout.fillHeight: true
            }
            
            // === More News ===
            Button {
                text: qsTr("More News")
                icon.name: "document-properties"
                Layout.preferredHeight: 32
                
                onClicked: bottomBar.moreNewsRequested()
                
                ToolTip.text: qsTr("View more news")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }
        }
    }
}
