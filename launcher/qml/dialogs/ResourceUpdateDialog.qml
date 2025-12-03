// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme.js" as Theme

Dialog {
    id: resourceUpdateDialog
    title: qsTr("Update Resources")
    modal: true
    width: 550
    height: 450
    standardButtons: Dialog.Cancel
    
    property var vm: null
    property var updatableResources: []
    property var selectedResources: []
    
    ColumnLayout {
        anchors.fill: parent
        spacing: Theme.spacingM
        
        // Header
        Label {
            Layout.fillWidth: true
            text: qsTr("The following resources have updates available:")
            color: Theme.textPrimary
        }
        
        // Select all
        RowLayout {
            Layout.fillWidth: true
            
            CheckBox {
                id: selectAllCheck
                text: qsTr("Select All")
                checked: selectedResources.length === updatableResources.length
                onClicked: {
                    if (checked) {
                        selectedResources = updatableResources.map(function(r) { return r.id })
                    } else {
                        selectedResources = []
                    }
                }
            }
            
            Item { Layout.fillWidth: true }
            
            Label {
                text: qsTr("%1 of %2 selected").arg(selectedResources.length).arg(updatableResources.length)
                color: Theme.textSecondary
            }
        }
        
        // Resources list
        Frame {
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            ListView {
                id: resourcesList
                anchors.fill: parent
                clip: true
                model: updatableResources
                spacing: 4
                
                delegate: Rectangle {
                    width: resourcesList.width
                    height: 64
                    radius: 6
                    color: selectedResources.indexOf(modelData.id) >= 0 ? Theme.accent + "15" : "transparent"
                    border.color: selectedResources.indexOf(modelData.id) >= 0 ? Theme.accent : "transparent"
                    border.width: 1
                    
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: Theme.spacingS
                        spacing: Theme.spacingM
                        
                        CheckBox {
                            checked: selectedResources.indexOf(modelData.id) >= 0
                            onClicked: {
                                var newSelected = selectedResources.slice()
                                var idx = newSelected.indexOf(modelData.id)
                                if (checked && idx < 0) {
                                    newSelected.push(modelData.id)
                                } else if (!checked && idx >= 0) {
                                    newSelected.splice(idx, 1)
                                }
                                selectedResources = newSelected
                            }
                        }
                        
                        // Icon
                        Rectangle {
                            Layout.preferredWidth: 44
                            Layout.preferredHeight: 44
                            radius: 6
                            color: Theme.backgroundAlt
                            
                            Image {
                                anchors.fill: parent
                                anchors.margins: 4
                                source: modelData.iconUrl || ""
                                fillMode: Image.PreserveAspectFit
                            }
                        }
                        
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2
                            
                            Label {
                                text: modelData.name || qsTr("Unknown")
                                color: Theme.textPrimary
                                font.bold: true
                            }
                            
                            RowLayout {
                                spacing: Theme.spacingS
                                
                                Label {
                                    text: modelData.currentVersion || ""
                                    color: Theme.textSecondary
                                    font.pointSize: Theme.fontSizeSmall
                                }
                                
                                Label {
                                    text: "→"
                                    color: Theme.accent
                                }
                                
                                Label {
                                    text: modelData.newVersion || ""
                                    color: Theme.accent
                                    font.pointSize: Theme.fontSizeSmall
                                    font.bold: true
                                }
                            }
                        }
                        
                        // Changelog button
                        ToolButton {
                            icon.name: "help-about"
                            ToolTip.text: qsTr("View Changelog")
                            ToolTip.visible: hovered
                            onClicked: {
                                changelogDialog.resourceName = modelData.name
                                changelogDialog.changelog = modelData.changelog || qsTr("No changelog available")
                                changelogDialog.open()
                            }
                        }
                    }
                }
                
                ScrollBar.vertical: ScrollBar {}
            }
            
            Label {
                anchors.centerIn: parent
                text: qsTr("No updates available")
                color: Theme.textSecondary
                visible: updatableResources.length === 0
            }
        }
        
        // Update button
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingM
            
            Button {
                text: qsTr("Check for Updates")
                icon.name: "view-refresh"
                onClicked: {
                    if (vm) vm.checkForUpdates()
                }
            }
            
            Item { Layout.fillWidth: true }
            
            Button {
                text: qsTr("Update Selected (%1)").arg(selectedResources.length)
                highlighted: true
                enabled: selectedResources.length > 0
                onClicked: {
                    if (vm) {
                        vm.updateResources(selectedResources)
                        resourceUpdateDialog.accept()
                    }
                }
            }
        }
    }
    
    // Changelog popup dialog
    Dialog {
        id: changelogDialog
        title: qsTr("Changelog - %1").arg(resourceName)
        modal: true
        width: 400
        height: 300
        standardButtons: Dialog.Ok
        
        property string resourceName: ""
        property string changelog: ""
        
        ScrollView {
            anchors.fill: parent
            
            TextArea {
                readOnly: true
                text: changelogDialog.changelog
                wrapMode: Text.WordWrap
                color: Theme.textPrimary
            }
        }
    }
}
