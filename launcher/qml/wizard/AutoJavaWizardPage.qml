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

Rectangle {
    id: autoJavaPage
    color: ThemeColors.background
    
    property bool autoDownloadEnabled: true
    
    signal settingsChanged()
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingL
        spacing: Theme.spacingM
        
        // Title
        Label {
            text: qsTr("New Feature Alert!")
            font.pixelSize: 18
            font.bold: true
            color: ThemeColors.text
        }
        
        // Description
        Label {
            Layout.fillWidth: true
            text: qsTr("We've added a feature to automatically download the correct Java version for each version of Minecraft (this can be changed in the Java Settings). Would you like to enable or disable this feature?")
            color: ThemeColors.text
            wrapMode: Text.WordWrap
        }
        
        // Separator
        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: ThemeColors.border
        }
        
        // Options
        ColumnLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingS
            
            RadioButton {
                id: enableRadio
                text: qsTr("Enable Auto-Download")
                checked: autoDownloadEnabled
                onCheckedChanged: {
                    if (checked) {
                        autoDownloadEnabled = true
                        settingsChanged()
                    }
                }
            }
            
            RadioButton {
                text: qsTr("Disable Auto-Download")
                checked: !autoDownloadEnabled
                onCheckedChanged: {
                    if (checked) {
                        autoDownloadEnabled = false
                        settingsChanged()
                    }
                }
            }
        }
        
        Item { Layout.fillHeight: true }
    }
}
