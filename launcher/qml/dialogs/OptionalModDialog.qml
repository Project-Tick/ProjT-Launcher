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
import "../../Theme.js" as Theme

Dialog {
    id: optionalModDialog
    title: qsTr("Select Optional Mods")
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel
    width: 550
    height: 350
    
    property var optionalMods: []  // Array of {name: "", description: "", checked: true}
    
    signal modsSelected(var selectedMods)
    
    ColumnLayout {
        anchors.fill: parent
        spacing: Theme.spacingM
        
        // Mod list
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: ThemeColors.backgroundAlt
            border.color: ThemeColors.border
            radius: Theme.radiusS
            
            ListView {
                id: modList
                anchors.fill: parent
                anchors.margins: 1
                clip: true
                model: optionalMods
                
                delegate: Rectangle {
                    width: modList.width
                    height: 50
                    color: index % 2 === 0 ? "transparent" : ThemeColors.surface
                    
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: Theme.spacingS
                        spacing: Theme.spacingM
                        
                        CheckBox {
                            id: modCheckbox
                            checked: modelData.checked !== false
                            onCheckedChanged: {
                                var mods = optionalMods.slice()
                                mods[index].checked = checked
                                optionalMods = mods
                            }
                        }
                        
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2
                            
                            Label {
                                text: modelData.name || ""
                                color: ThemeColors.text
                                font.bold: true
                            }
                            
                            Label {
                                Layout.fillWidth: true
                                text: modelData.description || ""
                                color: ThemeColors.textSecondary
                                font.pixelSize: 11
                                elide: Text.ElideRight
                            }
                        }
                    }
                }
                
                ScrollBar.vertical: ScrollBar {}
            }
        }
        
        // Selection buttons
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingM
            
            Button {
                text: qsTr("Select All")
                onClicked: {
                    var mods = optionalMods.slice()
                    for (var i = 0; i < mods.length; i++) {
                        mods[i].checked = true
                    }
                    optionalMods = mods
                }
            }
            
            Button {
                text: qsTr("Deselect All")
                onClicked: {
                    var mods = optionalMods.slice()
                    for (var i = 0; i < mods.length; i++) {
                        mods[i].checked = false
                    }
                    optionalMods = mods
                }
            }
            
            Item { Layout.fillWidth: true }
            
            Label {
                text: qsTr("Unchecked mods will be disabled.")
                color: ThemeColors.textSecondary
                font.italic: true
            }
        }
    }
    
    onAccepted: {
        var selected = []
        for (var i = 0; i < optionalMods.length; i++) {
            if (optionalMods[i].checked) {
                selected.push(optionalMods[i])
            }
        }
        modsSelected(selected)
    }
}
