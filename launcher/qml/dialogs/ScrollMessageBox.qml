// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme.js" as Theme

Dialog {
    id: scrollMessageBox
    title: ""
    modal: true
    standardButtons: Dialog.Ok
    width: 500
    height: 400
    
    property string message: ""
    property string icon: "" // info, warning, error, question
    
    ColumnLayout {
        anchors.fill: parent
        spacing: Theme.spacingM
        
        // Icon and title row
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingM
            visible: icon.length > 0
            
            Rectangle {
                Layout.preferredWidth: 48
                Layout.preferredHeight: 48
                radius: 24
                color: {
                    if (icon === "info") return "#3b82f6"
                    if (icon === "warning") return "#f59e0b"
                    if (icon === "error") return "#ef4444"
                    if (icon === "question") return "#8b5cf6"
                    return Theme.accent
                }
                
                Label {
                    anchors.centerIn: parent
                    text: {
                        if (icon === "info") return "i"
                        if (icon === "warning") return "!"
                        if (icon === "error") return "×"
                        if (icon === "question") return "?"
                        return ""
                    }
                    color: "white"
                    font.bold: true
                    font.pointSize: 20
                }
            }
            
            Label {
                Layout.fillWidth: true
                text: scrollMessageBox.title
                color: Theme.textPrimary
                font.bold: true
                font.pointSize: Theme.fontSizeMedium
                wrapMode: Text.WordWrap
            }
        }
        
        // Scrollable message content
        Frame {
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            ScrollView {
                anchors.fill: parent
                clip: true
                
                TextArea {
                    id: messageText
                    readOnly: true
                    text: message
                    wrapMode: Text.WordWrap
                    color: Theme.textPrimary
                    selectByMouse: true
                    background: Rectangle {
                        color: "transparent"
                    }
                }
            }
        }
        
        // Copy button
        RowLayout {
            Layout.fillWidth: true
            
            Item { Layout.fillWidth: true }
            
            Button {
                text: qsTr("Copy to Clipboard")
                icon.name: "edit-copy"
                onClicked: {
                    if (ProjT) {
                        ProjT.copyToClipboard(message)
                    }
                }
            }
        }
    }
}
