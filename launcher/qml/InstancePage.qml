// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Instance Management Page
 *  
 *  Phase 11.B: Full QML instance list with feature parity to Widgets
 *  - Instance list with cards (InstanceDelegate)
 *  - Context menu for all actions
 *  - Create/Import/Rename/Delete dialogs
 *  - Busy overlay with progress
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "components"
import "Theme.js" as Theme

Rectangle {
    objectName: "instances"
    color: Theme.background
    width: parent ? parent.width : 640
    height: parent ? parent.height : 480
    
    readonly property var vm: ProjT.instancesVM
    
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
        anchors.margins: Theme.spacingM
        spacing: Theme.spacingS

        // === Page Header ===
        PageHeader {
            Layout.fillWidth: true
            title: qsTr("Instances")
            subtitle: vm ? qsTr("Managing %1 instances").arg(vm.totalCount) : qsTr("No instances")
        }

        // === Toolbar ===
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingM
            
            // Action buttons
            RowLayout {
                spacing: Theme.spacingS
                Layout.fillWidth: true
                
                Button {
                    text: qsTr("New")
                    implicitHeight: 34
                    implicitWidth: 90
                    onClicked: {
                        if (vm) vm.emitCreateInstanceRequested()
                    }
                }
                
                Button {
                    text: qsTr("Import")
                    implicitHeight: 34
                    implicitWidth: 90
                    onClicked: {
                        if (vm) vm.emitImportInstanceRequested()
                    }
                }
                
                ToolSeparator { }
                
                Button {
                    text: qsTr("Launch")
                    implicitHeight: 34
                    implicitWidth: 90
                    enabled: vm && !vm.busy && vm.canLaunchSelected
                    onClicked: vm ? vm.launchSelectedInstance() : undefined
                }
                
                Button {
                    text: qsTr("Refresh")
                    implicitHeight: 34
                    implicitWidth: 90
                    enabled: vm && !vm.busy
                    onClicked: vm ? vm.refreshInstances() : undefined
                }
                
                Button {
                    text: qsTr("Delete")
                    implicitHeight: 34
                    implicitWidth: 90
                    enabled: vm && !vm.busy && vm.canDeleteSelected
                    onClicked: deleteDialog.open()
                }
            }
            
            Item { Layout.fillWidth: true }
            
            // Search field
            TextField {
                id: searchField
                placeholderText: qsTr("Search instances...")
                implicitHeight: 34
                Layout.preferredWidth: 200
                onTextChanged: {
                    // Filter instances based on search text
                    if (vm && text.length > 0) {
                        // Filter in QML - show only matching instances
                        instanceList.model = vm.instanceIds.filter(function(id, index) {
                            const name = vm.instanceNames[index]
                            return name.toLowerCase().includes(text.toLowerCase())
                        })
                    } else {
                        // Show all instances
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
                color: "#1a1d23"
                border.color: "#323742"
                border.width: 1
                radius: Theme.radius
            }
            
            padding: 0
            
            ListView {
                id: instanceList
                anchors.fill: parent
                clip: true
                spacing: Theme.spacingM
                
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
                    lastPlayedText: ""  // TODO: Get from instance metadata (requires InstanceListViewModel enhancement)
                    
                    onClicked: function(id) {
                        if (vm) vm.selectInstance(id)
                    }
                    
                    onDoubleClicked: function(id) {
                        if (vm) {
                            vm.selectInstance(id)
                            vm.launchSelectedInstance()
                        }
                    }
                    
                    onRightClicked: function(id, mouseX, mouseY) {
                        if (vm) vm.selectInstance(id)
                        contextMenu.popup(mouseX, mouseY)
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
                    color: Theme.textSecondary
                    font.pointSize: 14
                    horizontalAlignment: Text.AlignHCenter
                }
                
                // === Busy Overlay ===
                Rectangle {
                    anchors.fill: parent
                    color: "#000000"
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
                            color: Theme.textPrimary
                            font.pointSize: 12
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                    }
                }
            }
        }
    }

    // === Context Menu (Phase 11.B) ===
    InstanceContextMenu {
        id: contextMenu
        instanceId: vm ? vm.selectedInstanceId : ""
        canLaunch: vm ? vm.canLaunchSelected : false
        isRunning: vm ? vm.isSelectedRunning : false
        
        onLaunch: vm ? vm.launchSelectedInstance() : undefined
        onEditSettings: settingsWindow.openForInstance(vm.selectedInstanceId)
        onRename: renameDialog.open()
        onDuplicate: duplicateDialog.open()
        onOpenFolder: {
            // Open instance folder in file manager
            if (vm && vm.selectedInstanceId) {
                console.log("[InstancePage] Opening folder for instance:", vm.selectedInstanceId)
                if (vm.openInstanceFolder) {
                    vm.openInstanceFolder(vm.selectedInstanceId)
                }
            }
        }
        onBackup: {
            // Show backup dialog
            if (vm && vm.selectedInstanceId) {
                console.log("[InstancePage] Backup requested for:", vm.selectedInstanceId)
                backupDialog.open()
            }
        }
        onExportInstance: {
            // Show export dialog
            if (vm && vm.selectedInstanceId) {
                console.log("[InstancePage] Export requested for:", vm.selectedInstanceId)
                exportDialog.open()
            }
        }
        onDeleteInstance: deleteDialog.open()
        onCreateNew: newInstanceDialog.open()
        onImportInstance: importDialog.open()
    }

    // === Dialogs ===
    
    // Rename Dialog
    Dialog {
        id: renameDialog
        modal: true
        focus: true
        standardButtons: Dialog.Ok | Dialog.Cancel
        title: qsTr("Rename Instance")
        
        width: 360
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        
        property alias newName: nameField.text
        
        contentItem: ColumnLayout {
            anchors.fill: parent
            anchors.margins: Theme.spacingM
            spacing: Theme.spacingM
            
            Label {
                text: qsTr("New instance name:")
                color: Theme.textPrimary
            }
            
            TextField {
                id: nameField
                Layout.fillWidth: true
                text: selectedInstanceName
                placeholderText: qsTr("Enter new name...")
                focus: true
                Component.onCompleted: selectAll()
                
                Keys.onReturnPressed: renameDialog.accept()
                Keys.onEscapePressed: renameDialog.reject()
            }
        }
        
        onAccepted: {
            if (vm && newName.length > 0) {
                vm.renameSelectedInstance(newName)
                nameField.clear()
            }
        }
    }

    // Duplicate Dialog
    Dialog {
        id: duplicateDialog
        modal: true
        focus: true
        standardButtons: Dialog.Ok | Dialog.Cancel
        title: qsTr("Duplicate Instance")
        
        width: 360
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        
        property alias newName: dupNameField.text
        
        contentItem: ColumnLayout {
            anchors.fill: parent
            anchors.margins: Theme.spacingM
            spacing: Theme.spacingM
            
            Label {
                text: qsTr("New instance name:")
                color: Theme.textPrimary
            }
            
            TextField {
                id: dupNameField
                Layout.fillWidth: true
                text: selectedInstanceName + qsTr(" Copy")
                placeholderText: qsTr("Enter new name...")
                focus: true
                Component.onCompleted: selectAll()
                
                Keys.onReturnPressed: duplicateDialog.accept()
                Keys.onEscapePressed: duplicateDialog.reject()
            }
        }
        
        onAccepted: {
            if (vm && newName.length > 0) {
                vm.duplicateSelectedInstance(newName)
                dupNameField.clear()
            }
        }
    }

    // Delete Confirmation Dialog
    Dialog {
        id: deleteDialog
        modal: true
        focus: true
        standardButtons: Dialog.Yes | Dialog.No
        title: qsTr("Delete Instance")
        
        width: 380
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        
        contentItem: ColumnLayout {
            anchors.fill: parent
            anchors.margins: Theme.spacingM
            spacing: Theme.spacingM
            
            Label {
                wrapMode: Text.WordWrap
                text: selectedInstanceName.length > 0
                      ? qsTr("Delete \"%1\"?\n\nThis action cannot be undone.").arg(selectedInstanceName)
                      : qsTr("Delete the selected instance?\n\nThis action cannot be undone.")
                color: "#ff6b6b"  // Red warning color
                font.pointSize: 11
                Layout.fillWidth: true
            }
        }
        
        onAccepted: {
            if (vm) {
                vm.deleteSelectedInstance()
            }
        }
    }

    // New Instance Dialog
    Dialog {
        id: newInstanceDialog
        modal: true
        title: qsTr("Create New Instance")
        width: 450
        height: 280
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        standardButtons: Dialog.Ok | Dialog.Cancel
        
        contentItem: ColumnLayout {
            anchors.fill: parent
            anchors.margins: Theme.spacingM
            spacing: Theme.spacingM
            
            Label {
                text: qsTr("Instance name:")
                color: Theme.textPrimary
            }
            
            TextField {
                id: newNameField
                Layout.fillWidth: true
                placeholderText: qsTr("My Instance")
            }
            
            Label {
                text: qsTr("Version:")
                color: Theme.textPrimary
                topPadding: Theme.spacingM
            }
            
            ComboBox {
                id: versionCombo
                Layout.fillWidth: true
                // TODO: Populate from C++ VersionListViewModel (vm.availableVersions)
                model: vm && vm.availableVersions ? vm.availableVersions : ["Latest", "1.20.1", "1.19.2"]
                currentIndex: 0
            }
            
            Item { Layout.fillHeight: true }
        }
        
        onAccepted: {
            if (vm && newNameField.text.length > 0) {
                console.log("[InstancePage] Creating new instance:", newNameField.text)
                vm.createNewInstance(newNameField.text, versionCombo.currentText)
                newNameField.clear()
            }
        }
    }

    // Import Instance Dialog
    Dialog {
        id: importDialog
        modal: true
        title: qsTr("Import Instance")
        width: 450
        height: 280
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        standardButtons: Dialog.Ok | Dialog.Cancel
        
        contentItem: ColumnLayout {
            anchors.fill: parent
            anchors.margins: Theme.spacingM
            spacing: Theme.spacingM
            
            Label {
                text: qsTr("Import from:")
                color: Theme.textPrimary
            }
            
            RowLayout {
                Layout.fillWidth: true
                TextField {
                    id: importPathField
                    Layout.fillWidth: true
                    placeholderText: qsTr("e.g. /path/to/instance or instance.zip")
                    selectByMouse: true
                }
                Button {
                    text: qsTr("Browse...")
                    onClicked: {
                        if (ProjT.launcherVM) {
                            var path = ProjT.launcherVM.browseForFile(qsTr("Select Instance Archive"), qsTr("Zip Files (*.zip);;All Files (*)"))
                            if (path.length > 0) {
                                importPathField.text = path
                            }
                        }
                    }
                }
            }
            
            Label {
                text: qsTr("Instance name:")
                color: Theme.textPrimary
                topPadding: Theme.spacingM
            }
            
            TextField {
                id: importNameField
                Layout.fillWidth: true
                placeholderText: qsTr("Imported Instance")
            }
            
            Item { Layout.fillHeight: true }
        }
        
        onAccepted: {
            if (vm && importPathField.text.length > 0) {
                console.log("[InstancePage] Importing from:", importPathField.text)
                vm.importInstance(importPathField.text, importNameField.text)
            }
        }
    }

    // Backup Instance Dialog
    Dialog {
        id: backupDialog
        modal: true
        title: qsTr("Backup Instance")
        width: 450
        height: 250
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        standardButtons: Dialog.Ok | Dialog.Cancel
        
        contentItem: ColumnLayout {
            anchors.fill: parent
            anchors.margins: Theme.spacingM
            spacing: Theme.spacingM
            
            Label {
                text: qsTr("Backup name:")
                color: Theme.textPrimary
            }
            
            TextField {
                id: backupNameField
                Layout.fillWidth: true
                placeholderText: qsTr("Instance Backup - ") + new Date().toLocaleDateString()
            }
            
            Label {
                text: qsTr("Include mods and worlds:")
                color: Theme.textPrimary
                topPadding: Theme.spacingM
            }
            
            CheckBox {
                text: qsTr("Save complete backup")
                checked: true
            }
            
            Item { Layout.fillHeight: true }
        }
        
        onAccepted: {
            if (vm && backupNameField.text.length > 0) {
                console.log("[InstancePage] Backing up to:", backupNameField.text)
                vm.backupInstance(vm.selectedInstanceId, backupNameField.text)
                backupNameField.clear()
            }
        }
    }

    // Export Dialog
    Dialog {
        id: exportDialog
        modal: true
        title: qsTr("Export Instance")
        width: 450
        height: 280
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        standardButtons: Dialog.Ok | Dialog.Cancel
        
        contentItem: ColumnLayout {
            anchors.fill: parent
            anchors.margins: Theme.spacingM
            spacing: Theme.spacingM
            
            Label {
                text: qsTr("Export location:")
                color: Theme.textPrimary
            }
            
            RowLayout {
                Layout.fillWidth: true
                TextField {
                    id: exportPathField
                    Layout.fillWidth: true
                    placeholderText: qsTr("e.g. /home/user/backups")
                    selectByMouse: true
                }
                Button {
                    text: qsTr("Browse...")
                    onClicked: {
                        if (ProjT.launcherVM) {
                            var path = ProjT.launcherVM.browseForDirectory(qsTr("Select Export Location"))
                            if (path.length > 0) {
                                exportPathField.text = path
                            }
                        }
                    }
                }
            }
            
            Label {
                text: qsTr("Export as:")
                color: Theme.textPrimary
                topPadding: Theme.spacingM
            }
            
            ComboBox {
                id: exportFormatCombo
                Layout.fillWidth: true
                model: [".zip archive", ".tar.gz archive", "Copy folder"]
                currentIndex: 0
            }
            
            Item { Layout.fillHeight: true }
        }
        
        onAccepted: {
            if (vm && exportPathField.text.length > 0) {
                console.log("[InstancePage] Exporting to:", exportPathField.text)
                vm.exportInstance(vm.selectedInstanceId, exportPathField.text, exportFormatCombo.currentText)
            }
        }
    }

    // Settings Window - Opens instance settings
    Item {
        id: settingsWindow
        
        function openForInstance(instanceId) {
            if (instanceId && instanceId.length > 0) {
                console.log("[InstancePage] Opening settings for instance:", instanceId)
                // Switch to SettingsPage with this instance selected
                if (ProjT && ProjT.launcherVM) {
                    ProjT.launcherVM.setCurrentPage(2)  // 2 = Settings page
                }
            }
        }
    }
}

