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
    id: bottomBar
    height: 40
    width: parent.width
    
    // Theme binding for reactive updates
    property var themeVM: ProjT.themeVM
    property int _themeUpdateCount: 0
    
    color: {
        var _ = _themeUpdateCount
        return themeVM ? Qt.darker(themeVM.windowColor, 1.05) : ThemeColors.toolBar
    }
    
    Connections {
        target: themeVM
        function onThemeColorsChanged() {
            bottomBar._themeUpdateCount++
        }
    }
    
    // Properties
    property string statusMessage: qsTr("Ready")
    
    // Computed colors
    property color toolBarColor: {
        var _ = _themeUpdateCount
        return themeVM ? Qt.darker(themeVM.windowColor, 1.05) : ThemeColors.toolBar
    }
    property color borderColor: {
        var _ = _themeUpdateCount
        return themeVM ? Qt.darker(themeVM.windowColor, 1.2) : ThemeColors.border
    }
    property color textSecondaryColor: {
        var _ = _themeUpdateCount
        return themeVM ? Qt.darker(themeVM.textColor, 1.3) : ThemeColors.textSecondary
    }
    
    // Signals
    signal moreNewsRequested()
    
    Rectangle {
        anchors.fill: parent
        color: bottomBar.toolBarColor
        border.color: bottomBar.borderColor
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
                color: bottomBar.textSecondaryColor
                font.pointSize: 9
                Layout.fillWidth: true
                
                elide: Text.ElideRight
            }
            
            // Separator
            Rectangle {
                Layout.preferredWidth: 1
                Layout.preferredHeight: 20
                Layout.alignment: Qt.AlignVCenter
                color: bottomBar.borderColor
            }
            
            // === More News ===
            ThemedToolButton {
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
