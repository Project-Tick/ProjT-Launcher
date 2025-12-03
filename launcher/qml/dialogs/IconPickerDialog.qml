// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
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
                        color: selectedIcon === modelData ? Theme.accent : (parent.hovered ? Theme.surfaceHover : "transparent")
                        radius: Theme.radius
                        border.color: selectedIcon === modelData ? Theme.accent : "transparent"
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
                        color: selectedIcon === modelData ? Theme.accent : (parent.hovered ? Theme.surfaceHover : "transparent")
                        radius: Theme.radius
                        border.color: selectedIcon === modelData ? Theme.accent : "transparent"
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
                    // TODO: Open file dialog
                }
            }
            
            Item { Layout.fillWidth: true }
            
            Label {
                text: qsTr("Selected: %1").arg(selectedIcon)
                color: Theme.textSecondary
            }
        }
    }
}
