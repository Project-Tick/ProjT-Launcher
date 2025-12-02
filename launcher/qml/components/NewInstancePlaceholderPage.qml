// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  Placeholder page for New Instance dialog
 *  Used for pages not yet implemented in QML
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme.js" as Theme

Rectangle {
    id: placeholderPage
    color: Theme.background
    
    property string pageName: "Unknown"
    
    ColumnLayout {
        anchors.centerIn: parent
        spacing: Theme.spacingL
        
        Image {
            Layout.alignment: Qt.AlignHCenter
            source: "qrc:/icons/status/bug"
            width: 64
            height: 64
            fillMode: Image.PreserveAspectFit
            opacity: 0.5
        }
        
        Label {
            Layout.alignment: Qt.AlignHCenter
            text: pageName
            font.pointSize: 16
            font.bold: true
            color: Theme.textPrimary
        }
        
        Label {
            Layout.alignment: Qt.AlignHCenter
            text: qsTr("This page is not yet implemented in QML.\nPlease use the widget-based launcher for this feature.")
            color: Theme.textSecondary
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
        }
        
        Button {
            Layout.alignment: Qt.AlignHCenter
            text: qsTr("Open in Widget UI")
            visible: false // TODO: Implement fallback to widget UI
            onClicked: {
                // Would open widget version
            }
        }
    }
}
