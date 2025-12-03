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
        anchors.margins: Theme.spacingM
        spacing: Theme.spacingS
        
        // Header
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingS
            
            Label {
                text: qsTr("Instance Log")
                font.pointSize: 14
                font.bold: true
                color: Theme.textPrimary
            }
            
            Item { Layout.fillWidth: true }
            
            Button {
                text: qsTr("Copy")
                icon.name: "edit-copy"
                onClicked: {
                    if (vm) vm.copyLogToClipboard()
                }
            }
            
            Button {
                text: qsTr("Clear")
                icon.name: "edit-clear"
                onClicked: {
                    if (vm) vm.clearLog()
                }
            }
            
            Button {
                text: qsTr("Upload")
                icon.name: "upload-media"
                onClicked: {
                    if (vm) vm.uploadLog()
                }
            }
        }
        
        // Options
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingM
            
            CheckBox {
                id: wordWrapCheck
                text: qsTr("Word Wrap")
                checked: true
            }
            
            CheckBox {
                id: autoScrollCheck
                text: qsTr("Auto Scroll")
                checked: true
            }
            
            CheckBox {
                id: showTimestampsCheck
                text: qsTr("Timestamps")
                checked: false
            }
            
            Item { Layout.fillWidth: true }
            
            ComboBox {
                id: logLevelFilter
                model: [qsTr("All"), qsTr("Info"), qsTr("Warning"), qsTr("Error")]
                onCurrentIndexChanged: {
                    if (vm) vm.setLogLevelFilter(currentIndex)
                }
            }
        }
        
        // Search
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingS
            
            TextField {
                id: searchField
                Layout.fillWidth: true
                placeholderText: qsTr("Search log...")
                onTextChanged: {
                    if (vm) vm.searchLog(text)
                }
            }
            
            Button {
                text: qsTr("Find Previous")
                enabled: searchField.text.length > 0
                onClicked: {
                    if (vm) vm.findPreviousMatch()
                }
            }
            
            Button {
                text: qsTr("Find Next")
                enabled: searchField.text.length > 0
                onClicked: {
                    if (vm) vm.findNextMatch()
                }
            }
        }
        
        // Log viewer
        Frame {
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            ScrollView {
                id: logScrollView
                anchors.fill: parent
                clip: true
                
                TextArea {
                    id: logText
                    readOnly: true
                    wrapMode: wordWrapCheck.checked ? TextEdit.WrapAnywhere : TextEdit.NoWrap
                    text: vm ? vm.instanceLog : ""
                    font.family: "Noto Sans Mono"
                    font.pointSize: 10
                    color: Theme.textPrimary
                    selectByMouse: true
                    
                    background: Rectangle {
                        color: Theme.surfaceVariant
                    }
                }
            }
            
            // Auto-scroll on new content
            Connections {
                target: vm
                function onInstanceLogChanged() {
                    if (autoScrollCheck.checked) {
                        logScrollView.ScrollBar.vertical.position = 1.0 - logScrollView.ScrollBar.vertical.size
                    }
                }
            }
        }
        
        // Status
        RowLayout {
            Layout.fillWidth: true
            
            Label {
                text: vm && vm.instanceRunning ? qsTr("Instance is running...") : qsTr("Instance not running")
                color: vm && vm.instanceRunning ? Theme.success : Theme.textSecondary
            }
            
            Item { Layout.fillWidth: true }
            
            Label {
                text: vm ? qsTr("%1 lines").arg(vm.logLineCount || 0) : ""
                color: Theme.textSecondary
            }
        }
    }
}
