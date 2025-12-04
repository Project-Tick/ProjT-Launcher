// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme.js" as Theme

Rectangle {
    id: modsPage
    color: Theme.background
    
    property var vm: ProjT.instanceVM
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingM
        spacing: Theme.spacingS
        
        // Header with actions
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingS
            
            Label {
                text: qsTr("Mods")
                font.pointSize: 14
                font.bold: true
                color: Theme.textPrimary
            }
            
            Item { Layout.fillWidth: true }
            
            Button {
                text: qsTr("Add")
                icon.name: "list-add"
                onClicked: addModDialog.open()
            }
            
            Button {
                text: qsTr("Download")
                icon.name: "download"
                onClicked: {
                    if (vm) vm.openModDownload()
                }
            }
            
            Button {
                text: qsTr("Refresh")
                icon.name: "view-refresh"
                onClicked: {
                    if (vm) vm.refreshMods()
                }
            }
        }
        
        // Search and filter
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingS
            
            TextField {
                id: searchField
                Layout.fillWidth: true
                placeholderText: qsTr("Search mods...")
                onTextChanged: {
                    if (vm) vm.filterMods(text)
                }
            }
            
            CheckBox {
                text: qsTr("Show disabled")
                checked: true
                onCheckedChanged: {
                    if (vm) vm.setShowDisabledMods(checked)
                }
            }
        }
        
        // Mods list
        Frame {
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            ListView {
                id: modsList
                anchors.fill: parent
                clip: true
                model: vm ? vm.modsModel : []
                
                delegate: ItemDelegate {
                    width: modsList.width
                    height: 56
                    
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: Theme.spacingS
                        spacing: Theme.spacingS
                        
                        CheckBox {
                            checked: model.enabled !== false
                            onCheckedChanged: {
                                if (vm) vm.enableMod(index, checked)
                            }
                        }
                        
                        Image {
                            Layout.preferredWidth: 40
                            Layout.preferredHeight: 40
                            source: model.iconPath || ""
                            fillMode: Image.PreserveAspectFit
                            
                            Rectangle {
                                anchors.fill: parent
                                visible: parent.status !== Image.Ready
                                color: Theme.surfaceVariant
                                radius: 4
                                
                                Label {
                                    anchors.centerIn: parent
                                    text: "📦"
                                    font.pointSize: 16
                                }
                            }
                        }
                        
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2
                            
                            Label {
                                text: model.name || model.fileName || ""
                                color: Theme.textPrimary
                                font.bold: true
                                elide: Text.ElideRight
                                Layout.fillWidth: true
                            }
                            
                            Label {
                                text: model.version || ""
                                color: Theme.textSecondary
                                font.pointSize: 9
                                visible: text.length > 0
                            }
                        }
                        
                        Label {
                            text: model.provider || ""
                            color: Theme.accent
                            font.pointSize: 9
                        }
                        
                        ToolButton {
                            icon.name: "configure"
                            onClicked: modContextMenu.popup()
                            
                            Menu {
                                id: modContextMenu
                                
                                MenuItem {
                                    text: qsTr("View Details")
                                    onTriggered: {
                                        if (vm) vm.showModDetails(index)
                                    }
                                }
                                MenuItem {
                                    text: qsTr("Check for Updates")
                                    onTriggered: {
                                        if (vm) vm.checkModUpdate(index)
                                    }
                                }
                                MenuSeparator {}
                                MenuItem {
                                    text: qsTr("Open Folder")
                                    onTriggered: {
                                        if (vm) vm.openModFolder(index)
                                    }
                                }
                                MenuSeparator {}
                                MenuItem {
                                    text: qsTr("Delete")
                                    onTriggered: {
                                        deleteModDialog.modIndex = index
                                        deleteModDialog.modName = model.name || model.fileName
                                        deleteModDialog.open()
                                    }
                                }
                            }
                        }
                    }
                }
                
                ScrollBar.vertical: ScrollBar {}
            }
            
            Label {
                anchors.centerIn: parent
                visible: modsList.count === 0
                text: qsTr("No mods installed.\nClick 'Add' or 'Download' to add mods.")
                color: Theme.textSecondary
                horizontalAlignment: Text.AlignHCenter
            }
        }
        
        // Status bar
        RowLayout {
            Layout.fillWidth: true
            
            Label {
                text: vm ? qsTr("%1 mods").arg(vm.modsCount || 0) : ""
                color: Theme.textSecondary
            }
            
            Item { Layout.fillWidth: true }
            
            Button {
                text: qsTr("Check All Updates")
                flat: true
                onClicked: {
                    if (vm) vm.checkAllModUpdates()
                }
            }
        }
    }
    
    // Add mod dialog
    Dialog {
        id: addModDialog
        title: qsTr("Add Mod")
        modal: true
        standardButtons: Dialog.Ok | Dialog.Cancel
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        width: 400
        
        ColumnLayout {
            anchors.fill: parent
            spacing: Theme.spacingM
            
            Label {
                text: qsTr("Select mod files to add:")
                color: Theme.textPrimary
            }
            
            Button {
                text: qsTr("Browse...")
                Layout.fillWidth: true
                onClicked: {
                    if (vm) vm.browseForMods()
                }
            }
            
            Label {
                text: qsTr("Or drag and drop mod files here")
                color: Theme.textSecondary
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
            }
        }
        
        onAccepted: {
            // Handle file selection
        }
    }
    
    // Delete confirmation
    Dialog {
        id: deleteModDialog
        title: qsTr("Delete Mod")
        modal: true
        standardButtons: Dialog.Yes | Dialog.No
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        
        property int modIndex: -1
        property string modName: ""
        
        Label {
            text: qsTr("Delete '%1'?\n\nThis cannot be undone.").arg(deleteModDialog.modName)
            color: Theme.error
            wrapMode: Text.WordWrap
        }
        
        onAccepted: {
            if (vm && modIndex >= 0) {
                vm.removeMod(modIndex)
            }
        }
    }
}
