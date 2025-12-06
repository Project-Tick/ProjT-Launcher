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

Dialog {
    id: iconPickerDialog
    title: qsTr("Select Icon")
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel
    width: 400
    height: 450
    
    property string selectedIcon: "default"
    property var customIcons: []
    
    signal iconSelected(string iconKey)
    
    onAccepted: {
        iconSelected(selectedIcon)
    }
    
    ColumnLayout {
        anchors.fill: parent
        spacing: Theme.spacingM
        
        // Built-in icons
        GroupBox {
            Layout.fillWidth: true
            Layout.fillHeight: true
            title: qsTr("Built-in Icons")
            
            GridView {
                id: iconGrid
                anchors.fill: parent
                cellWidth: 56
                cellHeight: 56
                clip: true
                
                model: [
                    "default", "bee", "brick", "chicken", "creeper", 
                    "diamond", "dirt", "enderman", "enderpearl", "flame",
                    "fox", "gear", "herobrine", "magitech", "meat",
                    "modrinth", "netherstar", "planks", "skeleton", "squarecreeper",
                    "steve", "stone", "ftb_glow", "ftb_logo", "flame"
                ]
                
                delegate: Button {
                    width: 52
                    height: 52
                    
                    background: Rectangle {
                        color: selectedIcon === modelData ? ThemeColors.accent : (parent.hovered ? ThemeColors.surfaceHover : "transparent")
                        radius: Theme.radius
                        border.color: selectedIcon === modelData ? ThemeColors.accent : "transparent"
                        border.width: 2
                    }
                    
                    Image {
                        anchors.centerIn: parent
                        width: 40
                        height: 40
                        source: "qrc:/icons/multimc/scalable/instances/" + modelData + ".svg"
                        fillMode: Image.PreserveAspectFit
                    }
                    
                    onClicked: selectedIcon = modelData
                }
                
                ScrollBar.vertical: ScrollBar {}
            }
        }
        
        // Custom icons
        GroupBox {
            Layout.fillWidth: true
            Layout.preferredHeight: 100
            title: qsTr("Custom Icons")
            visible: customIcons.length > 0
            
            GridView {
                anchors.fill: parent
                cellWidth: 56
                cellHeight: 56
                clip: true
                model: customIcons
                
                delegate: Button {
                    width: 52
                    height: 52
                    
                    background: Rectangle {
                        color: selectedIcon === modelData ? ThemeColors.accent : (parent.hovered ? ThemeColors.surfaceHover : "transparent")
                        radius: Theme.radius
                        border.color: selectedIcon === modelData ? ThemeColors.accent : "transparent"
                        border.width: 2
                    }
                    
                    Image {
                        anchors.centerIn: parent
                        width: 40
                        height: 40
                        source: "file://" + modelData
                        fillMode: Image.PreserveAspectFit
                    }
                    
                    onClicked: selectedIcon = modelData
                }
            }
        }
        
        // Add custom icon
        RowLayout {
            Layout.fillWidth: true
            
            Button {
                text: qsTr("Add Custom Icon...")
                onClicked: {
                    if (ProjT && ProjT.launcherVM && ProjT.launcherVM.browseForFile) {
                        var filter = qsTr("Image files (*.png *.jpg *.jpeg *.svg *.ico);;All files (*)")
                        var path = ProjT.launcherVM.browseForFile(qsTr("Select Custom Icon"), filter)
                        if (path && path.length > 0) {
                            // Add to custom icons list
                            var newCustomIcons = customIcons.slice()
                            if (newCustomIcons.indexOf(path) === -1) {
                                newCustomIcons.push(path)
                            }
                            customIcons = newCustomIcons
                            // Select the newly added icon
                            selectedIcon = path
                        }
                    }
                }
            }
            
            Item { Layout.fillWidth: true }
            
            Label {
                text: qsTr("Selected: %1").arg(selectedIcon)
                color: ThemeColors.textSecondary
            }
        }
    }
}
