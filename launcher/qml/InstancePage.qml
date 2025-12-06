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
import ProjTLauncher 1.0
import "components"
import "Theme.js" as Theme

Rectangle {
    id: instancePage
    objectName: "instances"
    color: ThemeColors.background
    
    readonly property var vm: ProjT.instancesVM
    
    // Signals for parent components
    signal createNewInstance()
    
    property string selectedInstanceName: {
        if (!vm || !vm.instanceIds) return ""
        const idx = vm.instanceIds.indexOf(vm.selectedInstanceId)
        return idx >= 0 && idx < vm.instanceNames.length ? vm.instanceNames[idx] : ""
    }

    Component.onCompleted: {
        console.log("[InstancePage] Initialized - count:", vm ? vm.totalCount : 0)
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingS
        spacing: Theme.spacingXS

        // === Compact Toolbar (Search + Quick Actions) ===
        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 36
            spacing: Theme.spacingS
            
            // Instance count label
            Label {
                text: vm ? qsTr("%1 Instances").arg(vm.totalCount) : qsTr("No instances")
                color: ThemeColors.text
                font.pointSize: 12
                font.bold: true
            }
            
            Item { Layout.fillWidth: true }
            
            // Quick action buttons (secondary, main actions in sidebar)
            Button {
                text: qsTr("Import")
                implicitHeight: 28
                implicitWidth: 60
                flat: true
                font.pointSize: 10
                onClicked: importDialog.open()
                
                ToolTip.text: qsTr("Import an existing instance")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }
            
            Button {
                text: qsTr("Refresh")
                implicitHeight: 28
                implicitWidth: 65
                flat: true
                font.pointSize: 10
                enabled: vm && !vm.busy
                onClicked: vm ? vm.refreshInstances() : undefined
                
                ToolTip.text: qsTr("Refresh instance list")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }
            
            // Search field
            TextField {
                id: searchField
                placeholderText: qsTr("Search...")
                implicitHeight: 28
                font.pointSize: 10
                Layout.preferredWidth: 150
                
                background: Rectangle {
                    radius: Theme.radius
                    color: ThemeColors.surface
                    border.color: searchField.focus ? ThemeColors.accent : ThemeColors.border
                    border.width: 1
                }
                
                onTextChanged: {
                    if (vm && text.length > 0) {
                        instanceList.model = vm.instanceIds.filter(function(id, index) {
                            const name = vm.instanceNames[index]
                            return name.toLowerCase().includes(text.toLowerCase())
                        })
                    } else {
                        instanceList.model = vm ? vm.instanceIds : []
                    }
                }
            }
        }

        // === Instance List ===
        Frame {
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            background: Rectangle {
                color: ThemeColors.backgroundAlt
                border.color: ThemeColors.border
                border.width: 1
                radius: Theme.radius
            }
            
            padding: 0
            
            ListView {
                id: instanceList
                anchors.fill: parent
                clip: true
                spacing: Theme.spacingS
                
                model: vm ? vm.instanceIds : []
                currentIndex: vm ? vm.instanceIds.indexOf(vm.selectedInstanceId) : -1
                
                onCurrentIndexChanged: {
                    if (vm && currentIndex >= 0 && currentIndex < vm.instanceIds.length) {
                        const id = vm.instanceIds[currentIndex]
                        if (id !== vm.selectedInstanceId) {
                            vm.selectInstanceByIndex(currentIndex)
                        }
                    }
                }
                
                delegate: InstanceDelegate {
                    width: instanceList.width - (instanceList.ScrollBar.vertical.visible ? 16 : 0)
                    
                    instanceId: vm ? vm.instanceIds[index] : ""
                    instanceName: vm ? vm.instanceNames[index] : ""
                    instanceGroup: vm ? vm.instanceGroups[index] : ""
                    iconPath: vm ? vm.instanceIconPaths[index] : ""
                    isSelected: ListView.isCurrentItem
                    isRunning: vm && vm.isSelectedRunning && vm.instanceIds[index] === vm.selectedInstanceId ? vm.isSelectedRunning : false
                    lastPlayedText: vm && vm.instanceLastPlayed ? (vm.instanceLastPlayed[index] || "") : ""
                    
                    onClicked: function(id) {
                        if (vm) vm.selectInstance(id)
                    }
                    
                    onDoubleClicked: function(id) {
                        if (vm) {
                            vm.selectInstance(id)
                            vm.launchSelectedInstance()
                        }
                    }
                    
                    onRightClicked: function(id, globalX, globalY) {
                        if (vm) vm.selectInstance(id)
                        // Convert global screen coordinates to local page coordinates for popup
                        var localPos = instancePage.mapFromGlobal(globalX, globalY)
                        contextMenu.x = localPos.x
                        contextMenu.y = localPos.y
                        contextMenu.open()
                    }
                }
                
                // === Scrollbar ===
                ScrollBar.vertical: ScrollBar {
                    active: true
                    policy: ScrollBar.AsNeeded
                }
                
                // === Empty State ===
                Text {
                    visible: instanceList.count === 0
                    anchors.centerIn: parent
                    text: qsTr("No instances.\nClick 'New' to create one.")
                    color: ThemeColors.textSecondary
                    font.pointSize: 14
                    horizontalAlignment: Text.AlignHCenter
                }
                
                // === Busy Overlay ===
                Rectangle {
                    anchors.fill: parent
                    color: ThemeColors.background
                    opacity: vm && vm.busy ? 0.3 : 0
                    visible: opacity > 0
                    z: 10
                    
                    Behavior on opacity {
                        NumberAnimation { duration: 150 }
                    }
                    
                    Column {
                        anchors.centerIn: parent
                        spacing: Theme.spacingM
                        
                        BusyIndicator {
                            anchors.horizontalCenter: parent.horizontalCenter
                            running: vm ? vm.busy : false
                            visible: running
                        }
                        
                        Text {
                            text: vm && vm.busyReason ? vm.busyReason : qsTr("Loading...")
                            color: ThemeColors.text
                            font.pointSize: 12
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                    }
                }
            }
        }
    }

    // === Context Menu ===
    InstanceContextMenu {
        id: contextMenu
        instanceId: vm ? vm.selectedInstanceId : ""
        canLaunch: vm ? vm.canLaunchSelected : false
        isRunning: vm ? vm.isSelectedRunning : false
        
        onLaunch: {
            if (vm) {
                if (isRunning) {
                    vm.killSelectedInstance()
                } else {
                    vm.launchSelectedInstance()
                }
            }
        }
        onEditSettings: {
            // Open instance settings window via root
            var rootItem = instancePage
            while (rootItem.parent) {
                rootItem = rootItem.parent
            }
            if (rootItem.showInstanceSettingsWindow) {
                rootItem.showInstanceSettingsWindow(vm.selectedInstanceId)
            } else if (vm) {
                vm.openInstanceSettings()
            }
        }
        onRename: renameDialog.open()
        onDuplicate: duplicateDialog.open()
        onOpenFolder: {
            if (vm) vm.openInstanceFolder()
        }
        onBackup: {
            // Open backup dialog via root
            var rootItem = instancePage
            while (rootItem.parent) {
                rootItem = rootItem.parent
            }
            if (rootItem.showBackupDialog) {
                rootItem.showBackupDialog(vm.selectedInstanceId)
            } else if (vm) {
                vm.manageSelectedBackups()
            }
        }
        onExportInstance: {
            // Open export dialog via root
            var rootItem = instancePage
            while (rootItem.parent) {
                rootItem = rootItem.parent
            }
            if (rootItem.showExportDialog) {
                rootItem.showExportDialog(vm.selectedInstanceId)
            } else if (vm) {
                vm.exportSelectedInstance()
            }
        }
        onDeleteInstance: deleteDialog.open()
        onCreateNew: {
            instancePage.createNewInstance()
        }
        onImportInstance: importDialog.open()
    }

    // === Dialogs (for context menu actions) ===
    
    // Rename Dialog
    Dialog {
        id: renameDialog
        modal: true
        title: qsTr("Rename Instance")
        width: 360
        x: (instancePage.width - width) / 2
        y: (instancePage.height - height) / 2
        standardButtons: Dialog.Ok | Dialog.Cancel
        
        ColumnLayout {
            anchors.fill: parent
            spacing: Theme.spacingM
            
            Label {
                text: qsTr("New name:")
                color: ThemeColors.text
            }
            
            TextField {
                id: renameField
                Layout.fillWidth: true
                text: selectedInstanceName
                selectByMouse: true
                Keys.onReturnPressed: renameDialog.accept()
            }
        }
        
        onOpened: {
            renameField.text = selectedInstanceName
            renameField.selectAll()
            renameField.forceActiveFocus()
        }
        
        onAccepted: {
            if (vm && renameField.text.length > 0) {
                vm.renameSelectedInstance(renameField.text)
            }
        }
    }

    // Duplicate Dialog
    Dialog {
        id: duplicateDialog
        modal: true
        title: qsTr("Duplicate Instance")
        width: 360
        x: (instancePage.width - width) / 2
        y: (instancePage.height - height) / 2
        standardButtons: Dialog.Ok | Dialog.Cancel
        
        ColumnLayout {
            anchors.fill: parent
            spacing: Theme.spacingM
            
            Label {
                text: qsTr("New instance name:")
                color: ThemeColors.text
            }
            
            TextField {
                id: duplicateField
                Layout.fillWidth: true
                placeholderText: qsTr("Instance Copy")
                selectByMouse: true
                Keys.onReturnPressed: duplicateDialog.accept()
            }
        }
        
        onOpened: {
            duplicateField.text = selectedInstanceName + qsTr(" Copy")
            duplicateField.selectAll()
            duplicateField.forceActiveFocus()
        }
        
        onAccepted: {
            if (vm && duplicateField.text.length > 0) {
                vm.duplicateSelectedInstance(duplicateField.text)
            }
        }
    }

    // Delete Confirmation Dialog
    Dialog {
        id: deleteDialog
        modal: true
        title: qsTr("Delete Instance")
        width: 380
        x: (instancePage.width - width) / 2
        y: (instancePage.height - height) / 2
        standardButtons: Dialog.Yes | Dialog.No
        
        Label {
            text: qsTr("Delete \"%1\"?\n\nThis action cannot be undone.").arg(selectedInstanceName)
            color: ThemeColors.error
            wrapMode: Text.WordWrap
            width: parent.width
        }
        
        onAccepted: {
            if (vm) vm.deleteSelectedInstance()
        }
    }

    // Import Instance Dialog
    Dialog {
        id: importDialog
        modal: true
        title: qsTr("Import Instance")
        width: 450
        x: (instancePage.width - width) / 2
        y: (instancePage.height - height) / 2
        standardButtons: Dialog.Ok | Dialog.Cancel
        
        ColumnLayout {
            anchors.fill: parent
            spacing: Theme.spacingM
            
            Label {
                text: qsTr("Import from:")
                color: ThemeColors.text
            }
            
            RowLayout {
                Layout.fillWidth: true
                TextField {
                    id: importPathField
                    Layout.fillWidth: true
                    placeholderText: qsTr("/path/to/instance or instance.zip")
                    selectByMouse: true
                }
                Button {
                    text: qsTr("Browse...")
                    onClicked: {
                        var path = ProjT.launcherVM.browseForFile(
                            qsTr("Import Instance"),
                            qsTr("Archives (*.zip *.mrpack);;All files (*)")
                        )
                        if (path.length > 0) {
                            importPathField.text = path
                        }
                    }
                }
            }
            
            Label {
                text: qsTr("Instance name (optional):")
                color: ThemeColors.text
            }
            
            TextField {
                id: importNameField
                Layout.fillWidth: true
                placeholderText: qsTr("Leave empty to use archive name")
                selectByMouse: true
            }
        }
        
        onAccepted: {
            if (vm && importPathField.text.length > 0) {
                vm.importInstance(importPathField.text, importNameField.text)
                importPathField.text = ""
                importNameField.text = ""
            }
        }
    }

    // Export Dialog
    Dialog {
        id: exportDialog
        modal: true
        title: qsTr("Export Instance")
        width: 450
        x: (instancePage.width - width) / 2
        y: (instancePage.height - height) / 2
        standardButtons: Dialog.Ok | Dialog.Cancel
        
        ColumnLayout {
            anchors.fill: parent
            spacing: Theme.spacingM
            
            Label {
                text: qsTr("Export location:")
                color: ThemeColors.text
            }
            
            RowLayout {
                Layout.fillWidth: true
                TextField {
                    id: exportPathField
                    Layout.fillWidth: true
                    placeholderText: qsTr("/path/to/export")
                    selectByMouse: true
                }
                Button {
                    text: qsTr("Browse...")
                    onClicked: {
                        var path = ProjT.launcherVM.browseForDirectory(qsTr("Select Export Location"))
                        if (path.length > 0) {
                            exportPathField.text = path
                        }
                    }
                }
            }
            
            Label {
                text: qsTr("Format:")
                color: ThemeColors.text
            }
            
            ComboBox {
                id: exportFormatCombo
                Layout.fillWidth: true
                model: [".zip", ".mrpack", "Folder copy"]
            }
        }
        
        onAccepted: {
            if (vm && exportPathField.text.length > 0) {
                // For now, just open the export page in InstanceWindow
                vm.exportSelectedInstance()
            }
        }
    }
}