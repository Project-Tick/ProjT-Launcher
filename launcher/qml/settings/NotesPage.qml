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
 * Notes Page – Phase 11.C.3
 * Instance notes editor
 */

Rectangle {
    id: root
    objectName: "notesPage"
    color: Theme.background
    
    property var vm: ProjT.instanceVM
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingM
        spacing: Theme.spacingM
        
        Text {
            text: qsTr("Notes")
            font.pixelSize: 24
            font.weight: Font.Bold
            color: Theme.foreground
        }
        
        Text {
            text: root.vm && root.vm.instanceName ? root.vm.instanceName : qsTr("No instance selected")
            font.pixelSize: 14
            color: Theme.mutedForeground
            visible: root.vm !== null
        }
        
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            TextArea {
                id: notesTextArea
                placeholderText: qsTr("Add notes about this instance...")
                text: root.vm ? root.vm.notes : ""
                wrapMode: TextEdit.Wrap
                selectByMouse: true
                
                background: Rectangle {
                    color: Theme.surface0
                    border.color: notesTextArea.activeFocus ? Theme.accent : Theme.surface1
                    border.width: 1
                    radius: Theme.radiusS
                }
                
                color: Theme.foreground
                placeholderTextColor: Theme.mutedForeground
            }
        }
        
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingM
            
            Item { Layout.fillWidth: true }
            
            Button {
                text: qsTr("Clear")
                onClicked: notesTextArea.text = ""
                
                background: Rectangle {
                    color: parent.hovered ? Theme.surface1 : Theme.surface0
                    border.color: Theme.surface2
                    border.width: 1
                    radius: Theme.radiusS
                }
                
                contentItem: Text {
                    text: parent.text
                    color: Theme.foreground
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
            
            Button {
                text: qsTr("Save")
                onClicked: {
                    if (root.vm) {
                        root.vm.notes = notesTextArea.text;
                        root.vm.saveSettings();
                    }
                }
                
                background: Rectangle {
                    color: parent.hovered ? Qt.lighter(Theme.accent, 1.1) : Theme.accent
                    radius: Theme.radiusS
                }
                
                contentItem: Text {
                    text: parent.text
                    color: Theme.base
                    font.weight: Font.DemiBold
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
    }
}
