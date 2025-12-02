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
 * Servers Page – Phase 11.C.2
 * Lists and manages multiplayer servers
 */

Rectangle {
    objectName: "serversPage"
    color: Theme.background
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingM
        spacing: Theme.spacingM
        
        Text {
            text: qsTr("Servers")
            font.pixelSize: 20
            font.weight: Font.Bold
            color: Theme.foreground
        }
        
        RowLayout {
            spacing: Theme.spacingS
            Button { text: qsTr("Add"); width: 80 }
            Button { text: qsTr("Edit"); width: 80 }
            Button { text: qsTr("Delete"); width: 80 }
            Item { Layout.fillWidth: true }
        }
        
        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            
            model: 3
            delegate: Rectangle {
                width: parent.width
                height: 60
                color: Theme.background
                border.color: Theme.accent
                border.width: 1
                
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: Theme.spacingS
                    
                    ColumnLayout {
                        Layout.fillWidth: true
                        Text {
                            text: qsTr("Server %1").arg(index + 1)
                            color: Theme.foreground
                            font.bold: true
                        }
                        Text {
                            text: "mc.example.com:25565"
                            color: Theme.secondary
                            font.pixelSize: 12
                        }
                    }
                    
                    Text {
                        text: qsTr("Online")
                        color: "#4caf50"
                    }
                }
            }
        }
    }
}
