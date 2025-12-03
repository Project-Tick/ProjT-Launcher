// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme.js" as Theme

Dialog {
    id: versionSelectDialog
    title: qsTr("Select Version")
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel
    width: 500
    height: 500
    
    property string selectedVersion: ""
    property var vm: null
    property bool showReleases: true
    property bool showSnapshots: false
    property bool showOldVersions: false
    
    ColumnLayout {
        anchors.fill: parent
        spacing: Theme.spacingS
        
        // Filters
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingM
            
            CheckBox {
                text: qsTr("Releases")
                checked: showReleases
                onCheckedChanged: showReleases = checked
            }
            
            CheckBox {
                text: qsTr("Snapshots")
                checked: showSnapshots
                onCheckedChanged: showSnapshots = checked
            }
            
            CheckBox {
                text: qsTr("Old Versions")
                checked: showOldVersions
                onCheckedChanged: showOldVersions = checked
            }
            
            Item { Layout.fillWidth: true }
            
            Button {
                text: qsTr("Refresh")
                icon.name: "view-refresh"
                onClicked: {
                    if (vm) vm.refreshVersions()
                }
            }
        }
        
        // Search
        TextField {
            id: searchField
            Layout.fillWidth: true
            placeholderText: qsTr("Search versions...")
        }
        
        // Version list
        Frame {
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            ListView {
                id: versionsList
                anchors.fill: parent
                clip: true
                model: vm ? vm.filteredVersions : []
                
                delegate: ItemDelegate {
                    width: versionsList.width
                    highlighted: modelData === selectedVersion
                    
                    visible: {
                        // Filter by search
                        if (searchField.text.length > 0) {
                            if (!modelData.toLowerCase().includes(searchField.text.toLowerCase())) {
                                return false
                            }
                        }
                        return true
                    }
                    height: visible ? 36 : 0
                    
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: Theme.spacingS
                        spacing: Theme.spacingS
                        
                        Rectangle {
                            Layout.preferredWidth: 24
                            Layout.preferredHeight: 24
                            radius: 4
                            color: {
                                if (modelData.includes("snapshot") || modelData.includes("pre") || modelData.includes("rc")) {
                                    return "#f59e0b" // Snapshot - amber
                                }
                                if (modelData.startsWith("b") || modelData.startsWith("a") || modelData.includes("inf")) {
                                    return "#8b5cf6" // Old - purple
                                }
                                return "#22c55e" // Release - green
                            }
                            
                            Label {
                                anchors.centerIn: parent
                                text: {
                                    if (modelData.includes("snapshot") || modelData.includes("pre") || modelData.includes("rc")) return "S"
                                    if (modelData.startsWith("b") || modelData.startsWith("a")) return "O"
                                    return "R"
                                }
                                color: "white"
                                font.bold: true
                                font.pointSize: 10
                            }
                        }
                        
                        Label {
                            text: modelData
                            color: Theme.textPrimary
                            Layout.fillWidth: true
                        }
                    }
                    
                    onClicked: {
                        selectedVersion = modelData
                    }
                    
                    onDoubleClicked: {
                        selectedVersion = modelData
                        versionSelectDialog.accept()
                    }
                }
                
                ScrollBar.vertical: ScrollBar {}
            }
            
            BusyIndicator {
                anchors.centerIn: parent
                running: vm ? vm.loadingVersions : false
                visible: running
            }
        }
        
        // Selected version
        RowLayout {
            Layout.fillWidth: true
            
            Label {
                text: qsTr("Selected:")
                color: Theme.textSecondary
            }
            
            Label {
                text: selectedVersion.length > 0 ? selectedVersion : qsTr("None")
                color: selectedVersion.length > 0 ? Theme.accent : Theme.textSecondary
                font.bold: selectedVersion.length > 0
            }
            
            Item { Layout.fillWidth: true }
        }
    }
}
