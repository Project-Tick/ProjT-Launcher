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
    objectName: "notesPage"
    color: Theme.background
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingM
        spacing: Theme.spacingM
        
        Text {
            text: qsTr("Notes")
            font.pixelSize: 20
            font.weight: Font.Bold
            color: Theme.foreground
        }
        
        TextArea {
            Layout.fillWidth: true
            Layout.fillHeight: true
            placeholderText: qsTr("Add notes about this instance...")
            text: qsTr("This is a Fabric instance with mods.\nManaged by Project Tick Launcher.")
        }
        
        RowLayout {
            spacing: Theme.spacingS
            Button { text: qsTr("Save"); width: 80 }
            Button { text: qsTr("Clear"); width: 80 }
            Item { Layout.fillWidth: true }
        }
    }
}
