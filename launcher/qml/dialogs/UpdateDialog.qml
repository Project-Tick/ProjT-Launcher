// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme.js" as Theme

Dialog {
    id: updateDialog
    title: qsTr("Update Available")
    modal: true
    standardButtons: Dialog.Yes | Dialog.No
    width: 450
    height: 300
    
    property string currentVersion: ""
    property string newVersion: ""
    property string releaseNotes: ""
    property string downloadUrl: ""
    
    ColumnLayout {
        anchors.fill: parent
        spacing: Theme.spacingM
        
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingM
            
            Image {
                Layout.preferredWidth: 48
                Layout.preferredHeight: 48
                source: "qrc:/icons/multimc/scalable/instances/default.svg"
                fillMode: Image.PreserveAspectFit
            }
            
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 4
                
                Label {
                    text: qsTr("A new version is available!")
                    font.bold: true
                    font.pointSize: 12
                    color: Theme.textPrimary
                }
                
                Label {
                    text: qsTr("Current: %1 → New: %2").arg(currentVersion).arg(newVersion)
                    color: Theme.textSecondary
                }
            }
        }
        
        GroupBox {
            Layout.fillWidth: true
            Layout.fillHeight: true
            title: qsTr("Release Notes")
            
            ScrollView {
                anchors.fill: parent
                clip: true
                
                TextArea {
                    readOnly: true
                    text: releaseNotes
                    wrapMode: TextEdit.Wrap
                    textFormat: TextEdit.MarkdownText
                    color: Theme.textPrimary
                    
                    background: Rectangle {
                        color: "transparent"
                    }
                }
            }
        }
        
        Label {
            text: qsTr("Would you like to download and install the update?")
            color: Theme.textPrimary
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
        }
    }
    
    onAccepted: {
        Qt.openUrlExternally(downloadUrl)
    }
}
