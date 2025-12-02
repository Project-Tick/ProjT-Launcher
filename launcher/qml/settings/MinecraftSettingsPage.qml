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
 * Minecraft Settings Page – Phase 11.C.1
 * Configures Minecraft-specific settings
 */

Rectangle {
    objectName: "minecraftSettings"
    color: Theme.background
    
    Flickable {
        anchors.fill: parent
        anchors.margins: Theme.spacingM
        contentHeight: column.height
        
        ColumnLayout {
            id: column
            width: parent.width
            spacing: Theme.spacingM
            
            Text {
                text: qsTr("Minecraft Settings")
                font.pixelSize: 24
                font.weight: Font.Bold
                color: Theme.foreground
            }
            
            GroupBox {
                title: qsTr("Window")
                Layout.fillWidth: true
                
                GridLayout {
                    width: parent.width
                    columns: 2
                    columnSpacing: Theme.spacingM
                    rowSpacing: Theme.spacingS
                    
                    Text { text: qsTr("Width:"); color: Theme.foreground }
                    SpinBox {
                        from: 320
                        to: 7680
                        value: 1280
                        Layout.fillWidth: true
                    }
                    
                    Text { text: qsTr("Height:"); color: Theme.foreground }
                    SpinBox {
                        from: 240
                        to: 4320
                        value: 720
                        Layout.fillWidth: true
                    }
                    
                    Text { text: qsTr(""); color: Theme.foreground }
                    CheckBox { text: qsTr("Fullscreen") }
                }
            }
            
            GroupBox {
                title: qsTr("Rendering")
                Layout.fillWidth: true
                
                ColumnLayout {
                    width: parent.width
                    spacing: Theme.spacingS
                    
                    RowLayout {
                        Text { text: qsTr("Renderer:"); color: Theme.foreground }
                        ComboBox {
                            model: ["OpenGL", "Vulkan"]
                            Layout.fillWidth: true
                        }
                    }
                    
                    CheckBox { text: qsTr("VSync") }
                    CheckBox { text: qsTr("Fullscreen Windowed") }
                }
            }
            
            Item { Layout.fillHeight: true }
        }
    }
}
