// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme.js" as Theme

Dialog {
    id: aboutDialog
    title: qsTr("About ProjT Launcher")
    modal: true
    width: 500
    height: 450
    standardButtons: Dialog.Close
    
    property var appInfo: ProjT ? ProjT.appInfo : null
    
    ColumnLayout {
        anchors.fill: parent
        spacing: Theme.spacingM
        
        // Logo and title
        ColumnLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
            spacing: Theme.spacingS
            
            Rectangle {
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: 96
                Layout.preferredHeight: 96
                radius: 16
                color: Theme.accent
                
                Image {
                    anchors.fill: parent
                    anchors.margins: 8
                    source: "qrc:/launcher/icon.png"
                    fillMode: Image.PreserveAspectFit
                }
            }
            
            Label {
                text: "ProjT Launcher"
                color: Theme.textPrimary
                font.bold: true
                font.pointSize: Theme.fontSizeMedium + 4
                Layout.alignment: Qt.AlignHCenter
            }
            
            Label {
                text: qsTr("Version %1").arg(appInfo ? appInfo.version : "")
                color: Theme.textSecondary
                Layout.alignment: Qt.AlignHCenter
            }
            
            Label {
                text: appInfo ? appInfo.buildDate : ""
                color: Theme.textSecondary
                font.pointSize: Theme.fontSizeSmall
                Layout.alignment: Qt.AlignHCenter
            }
        }
        
        // Description
        Label {
            Layout.fillWidth: true
            text: qsTr("A free, open-source Minecraft launcher with support for mods, modpacks, and multiple instances.")
            color: Theme.textSecondary
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
        }
        
        // Links
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Links")
            
            RowLayout {
                anchors.fill: parent
                spacing: Theme.spacingM
                
                Button {
                    text: qsTr("Website")
                    flat: true
                    onClicked: Qt.openUrlExternally("https://github.com/Project-Tick/ProjT-Launcher")
                }
                
                Button {
                    text: qsTr("GitHub")
                    flat: true
                    onClicked: Qt.openUrlExternally("https://github.com/Project-Tick/ProjT-Launcher")
                }
                
                Button {
                    text: qsTr("Discord")
                    flat: true
                    onClicked: Qt.openUrlExternally("https://discord.gg/projt")
                }
                
                Button {
                    text: qsTr("Report Bug")
                    flat: true
                    onClicked: Qt.openUrlExternally("https://github.com/Project-Tick/ProjT-Launcher/issues")
                }
            }
        }
        
        // License
        GroupBox {
            Layout.fillWidth: true
            Layout.fillHeight: true
            title: qsTr("License")
            
            ScrollView {
                anchors.fill: parent
                clip: true
                
                TextArea {
                    readOnly: true
                    text: qsTr("ProjT Launcher is licensed under the GNU General Public License v3.0.\n\nThis program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.\n\nThis program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.")
                    wrapMode: Text.WordWrap
                    color: Theme.textSecondary
                    font.pointSize: Theme.fontSizeSmall
                    background: Rectangle {
                        color: "transparent"
                    }
                }
            }
        }
        
        // Credits
        Label {
            Layout.fillWidth: true
            text: qsTr("Based on PolyMC/Prism Launcher. Thanks to all contributors!")
            color: Theme.textSecondary
            font.pointSize: Theme.fontSizeSmall
            horizontalAlignment: Text.AlignHCenter
        }
    }
}
