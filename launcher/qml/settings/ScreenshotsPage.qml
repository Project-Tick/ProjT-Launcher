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

/**
 * Screenshots Page – Phase 11.C.2
 * Screenshots gallery
 */

Rectangle {
    objectName: "screenshotsPage"
    color: Theme.background
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingM
        spacing: Theme.spacingM
        
        Text {
            text: qsTr("Screenshots")
            font.pixelSize: 20
            font.weight: Font.Bold
            color: Theme.foreground
        }
        
        RowLayout {
            spacing: Theme.spacingS
            Button { text: qsTr("Open Folder"); width: 100 }
            Button { text: qsTr("Copy"); width: 80 }
            Item { Layout.fillWidth: true }
        }
        
        GridView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            cellWidth: 200
            cellHeight: 150
            
            model: 6
            delegate: Rectangle {
                width: 180
                height: 130
                color: Theme.secondary
                border.color: Theme.accent
                border.width: 1
                
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: Theme.spacingS
                    
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        color: "#333"
                        
                        Text {
                            anchors.centerIn: parent
                            text: qsTr("Screenshot")
                            color: Theme.foreground
                        }
                    }
                    
                    Text {
                        text: qsTr("2025-12-01")
                        color: Theme.foreground
                        font.pixelSize: 10
                    }
                }
            }
        }
    }
}
