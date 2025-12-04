// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme.js" as Theme

Rectangle {
    id: logPage
    color: Theme.background
    
    property var vm: ProjT.instanceVM
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 0
        spacing: 0
        
        // Top toolbar
        RowLayout {
            Layout.fillWidth: true
            Layout.margins: Theme.spacingS
            spacing: Theme.spacingM
            
            CheckBox {
                id: trackLogCheckbox
                text: qsTr("Keep updating")
                checked: true
            }
            
            CheckBox {
                id: wrapCheckbox
                text: qsTr("Wrap lines")
                checked: true
            }
            
            CheckBox {
                id: colorCheckbox
                text: qsTr("Color lines")
                checked: true
            }
            
            Item { Layout.fillWidth: true }
            
            Button {
                text: qsTr("&Copy")
                ToolTip.visible: hovered
                ToolTip.text: qsTr("Copy the whole log into the clipboard")
                onClicked: {
                    if (vm) vm.copyLogToClipboard()
                }
            }
            
            Button {
                text: qsTr("Upload")
                ToolTip.visible: hovered
                ToolTip.text: qsTr("Upload the log to the paste service configured in preferences")
                onClicked: {
                    if (vm) vm.uploadLog()
                }
            }
            
            Button {
                text: qsTr("Clear")
                ToolTip.visible: hovered
                ToolTip.text: qsTr("Clear the log")
                onClicked: {
                    if (vm) vm.clearLog()
                }
            }
        }
        
        // Log viewer
        ScrollView {
            id: logScrollView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            
            TextArea {
                id: logText
                readOnly: true
                wrapMode: wrapCheckbox.checked ? TextEdit.WrapAnywhere : TextEdit.NoWrap
                text: vm ? vm.instanceLog : ""
                font.family: "Noto Sans Mono"
                font.pointSize: 10
                color: Theme.textPrimary
                selectByMouse: true
                textFormat: colorCheckbox.checked ? TextEdit.RichText : TextEdit.PlainText
                
                background: Rectangle {
                    color: Theme.surfaceVariant
                }
            }
        }
        
        // Bottom search bar
        RowLayout {
            Layout.fillWidth: true
            Layout.margins: Theme.spacingS
            spacing: Theme.spacingS
            
            TextField {
                id: searchBar
                Layout.fillWidth: true
                placeholderText: qsTr("Search")
                onAccepted: {
                    if (vm) vm.findInLog(text)
                }
            }
            
            Button {
                text: qsTr("Find")
                onClicked: {
                    if (vm) vm.findInLog(searchBar.text)
                }
            }
            
            Rectangle {
                width: 1
                height: parent.height - 8
                color: Theme.border
            }
            
            Button {
                text: qsTr("Bottom")
                ToolTip.visible: hovered
                ToolTip.text: qsTr("Scroll all the way to bottom")
                onClicked: {
                    logScrollView.ScrollBar.vertical.position = 1.0 - logScrollView.ScrollBar.vertical.size
                }
            }
        }
        
        // Auto-scroll on new content
        Connections {
            target: vm
            ignoreUnknownSignals: true
            function onInstanceLogChanged() {
                if (trackLogCheckbox.checked) {
                    logScrollView.ScrollBar.vertical.position = 1.0 - logScrollView.ScrollBar.vertical.size
                }
            }
        }
    }
}
