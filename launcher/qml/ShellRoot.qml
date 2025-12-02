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
import QtQuick.Window 2.15
import ProjTLauncher 1.0
import "components"
import "Theme.js" as Theme

Rectangle {
    id: root
    color: Theme.background
    anchors.fill: parent
    
    // Sidebar width configuration
    property int sidebarWidth: 220
    property bool sidebarVisible: true
    
    // Page source mapping function
    function pageSource(page) {
        switch (page) {
        case LauncherViewModelEnums.Page.News:
            return "NewsPage.qml"
        case LauncherViewModelEnums.Page.Settings:
            return "SettingsPage.qml"
        case LauncherViewModelEnums.Page.About:
            return "AboutPage.qml"
        case LauncherViewModelEnums.Page.Logs:
            return "LogsPage.qml"
        case LauncherViewModelEnums.Page.Instances:
        default:
            return "InstancePage.qml"
        }
    }
    
    // Check if current page is Instances (sidebar only visible on Instances page)
    readonly property bool isInstancesPage: {
        if (!ProjT.launcherVM) return true
        return ProjT.launcherVM.currentPage === LauncherViewModelEnums.Page.Instances
    }
    
    Component.onCompleted: {
        console.log("[ShellRoot] Component loaded")
        console.log("[ShellRoot] LauncherVM available:", !!ProjT.launcherVM)
        console.log("[ShellRoot] InstancesVM available:", !!ProjT.instancesVM)
        
        // Initialize instance list from backend
        if (ProjT.instancesVM) {
            console.log("[ShellRoot] Refreshing instances list...")
            ProjT.instancesVM.refreshInstances()
        }
    }
    
    // === New Instance Dialog (shared) ===
    Dialog {
        id: newInstanceDialog
        title: qsTr("Create New Instance")
        modal: true
        standardButtons: Dialog.Ok | Dialog.Cancel
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        width: 400

        ColumnLayout {
            anchors.fill: parent
            spacing: 10

            Label {
                text: qsTr("Instance Name:")
                color: Theme.textPrimary
            }
            
            TextField {
                id: instanceNameField
                placeholderText: qsTr("My Instance")
                Layout.fillWidth: true
                selectByMouse: true
            }

            Label {
                text: qsTr("Version:")
                color: Theme.textPrimary
            }
            
            TextField {
                id: instanceVersionField
                placeholderText: qsTr("e.g. 1.20.1 (or leave empty for latest)")
                Layout.fillWidth: true
                selectByMouse: true
            }
        }

        onAccepted: {
            if (ProjT.instancesVM && instanceNameField.text.length > 0) {
                ProjT.instancesVM.createNewInstance(instanceNameField.text, instanceVersionField.text)
            }
            instanceNameField.text = ""
            instanceVersionField.text = ""
        }
    }
    
    // === Rename Dialog (for sidebar) ===
    Dialog {
        id: renameDialog
        title: qsTr("Rename Instance")
        modal: true
        standardButtons: Dialog.Ok | Dialog.Cancel
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        width: 360
        
        ColumnLayout {
            anchors.fill: parent
            spacing: 10
            
            Label {
                text: qsTr("New name:")
                color: Theme.textPrimary
            }
            
            TextField {
                id: renameField
                Layout.fillWidth: true
                text: instanceDetailSidebar.instanceName
                selectByMouse: true
            }
        }
        
        onAccepted: {
            if (ProjT.instancesVM && renameField.text.length > 0) {
                ProjT.instancesVM.renameSelectedInstance(renameField.text)
            }
        }
        
        onOpened: {
            renameField.text = instanceDetailSidebar.instanceName
            renameField.selectAll()
        }
    }
    
    // === Duplicate Dialog (for sidebar) ===
    Dialog {
        id: duplicateDialog
        title: qsTr("Duplicate Instance")
        modal: true
        standardButtons: Dialog.Ok | Dialog.Cancel
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        width: 360
        
        ColumnLayout {
            anchors.fill: parent
            spacing: 10
            
            Label {
                text: qsTr("New instance name:")
                color: Theme.textPrimary
            }
            
            TextField {
                id: duplicateNameField
                Layout.fillWidth: true
                placeholderText: qsTr("Instance Copy")
                selectByMouse: true
            }
        }
        
        onAccepted: {
            if (ProjT.instancesVM && duplicateNameField.text.length > 0) {
                ProjT.instancesVM.duplicateSelectedInstance(duplicateNameField.text)
            }
        }
        
        onOpened: {
            duplicateNameField.text = instanceDetailSidebar.instanceName + qsTr(" Copy")
            duplicateNameField.selectAll()
        }
    }
    
    // === Delete Confirmation Dialog (for sidebar) ===
    Dialog {
        id: deleteDialog
        title: qsTr("Delete Instance")
        modal: true
        standardButtons: Dialog.Yes | Dialog.No
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        width: 380
        
        Label {
            text: qsTr("Delete \"%1\"?\n\nThis action cannot be undone.").arg(instanceDetailSidebar.instanceName)
            color: "#ff6b6b"
            wrapMode: Text.WordWrap
            width: parent.width
        }
        
        onAccepted: {
            if (ProjT.instancesVM) {
                ProjT.instancesVM.deleteSelectedInstance()
            }
        }
    }

    // === MAIN LAYOUT ===
    ColumnLayout {
        anchors.fill: parent
        spacing: 0
        
        // ════════════════════════════════════════════════════════════
        // TOP BAR - Page Navigation
        // ════════════════════════════════════════════════════════════
        NavigationTopBar {
            id: navigationTopBar
            Layout.fillWidth: true
            Layout.preferredHeight: 48
            
            onPageRequested: function(page) {
                console.log("[ShellRoot] Page requested:", page)
                pageLoader.source = pageSource(page)
            }
            
            onCreateNewInstance: {
                newInstanceDialog.open()
            }
            
            onAccountsMenuRequested: {
                console.log("[ShellRoot] Accounts menu requested")
                // TODO: Open accounts management dialog
            }
        }
        
        // ════════════════════════════════════════════════════════════
        // UPPER BOTTOM BAR - News Feed / Update Info
        // ════════════════════════════════════════════════════════════
        UpperBottomBar {
            id: upperBottomBar
            Layout.fillWidth: true
            Layout.preferredHeight: 36
            
            onMoreNewsClicked: {
                if (ProjT.launcherVM) {
                    ProjT.launcherVM.currentPage = LauncherViewModelEnums.Page.News
                }
            }
            
            onUpdateClicked: {
                if (ProjT.launcherVM) {
                    ProjT.launcherVM.checkUpdates()
                }
            }
        }
        
        // ════════════════════════════════════════════════════════════
        // MAIN CONTENT AREA (PageArea + InstanceDetailSidebar)
        // ════════════════════════════════════════════════════════════
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0
            
            // === PAGE AREA (Left) ===
            Loader {
                id: pageLoader
                Layout.fillWidth: true
                Layout.fillHeight: true
                
                source: pageSource(ProjT.launcherVM ? 
                                   ProjT.launcherVM.currentPage : 
                                   LauncherViewModelEnums.Page.Instances)
                
                // Re-load when page changes from ViewModel
                Connections {
                    target: ProjT.launcherVM
                    function onCurrentPageChanged() {
                        pageLoader.source = pageSource(ProjT.launcherVM.currentPage)
                    }
                }
                
                asynchronous: true
                
                // Loading indicator
                BusyIndicator {
                    anchors.centerIn: parent
                    running: pageLoader.status === Loader.Loading
                    visible: running
                }
            }
            
            // === Resize Handle ===
            Rectangle {
                visible: isInstancesPage && sidebarVisible
                Layout.preferredWidth: 4
                Layout.fillHeight: true
                color: resizeArea.containsMouse || resizeArea.pressed ? Theme.accent : "#323742"
                
                Behavior on color { ColorAnimation { duration: 100 } }
                
                MouseArea {
                    id: resizeArea
                    anchors.fill: parent
                    cursorShape: Qt.SplitHCursor
                    hoverEnabled: true
                    
                    property int startX: 0
                    property int startWidth: 0
                    
                    onPressed: {
                        startX = mouseX
                        startWidth = sidebarWidth
                    }
                    
                    onPositionChanged: {
                        if (pressed) {
                            var delta = startX - mouseX
                            var newWidth = Math.max(180, Math.min(400, startWidth + delta))
                            sidebarWidth = newWidth
                        }
                    }
                }
            }
            
            // === INSTANCE DETAIL SIDEBAR (Right) ===
            InstanceDetailSidebar {
                id: instanceDetailSidebar
                visible: isInstancesPage && sidebarVisible
                Layout.preferredWidth: sidebarWidth
                Layout.fillHeight: true
                
                // Connect dialog signals
                onRenameRequested: renameDialog.open()
                onDeleteRequested: deleteDialog.open()
                onDuplicateRequested: duplicateDialog.open()
                onEditRequested: {
                    console.log("[ShellRoot] Edit instance settings requested")
                    if (ProjT.instancesVM) {
                        ProjT.instancesVM.openInstanceSettings()
                    }
                }
                onCreateShortcutRequested: {
                    console.log("[ShellRoot] Create shortcut requested")
                    // TODO: Implement shortcut creation
                }
            }
        }
        
        // ════════════════════════════════════════════════════════════
        // LOWER BOTTOM BAR - Global Runtime Status
        // ════════════════════════════════════════════════════════════
        LowerBottomBar {
            id: lowerBottomBar
            Layout.fillWidth: true
            Layout.preferredHeight: 32
        }
    }
}


