// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import ProjTLauncher 1.0
import "components"
import "Theme.js" as Theme

/**
 * Shell Root – Main Application Container
 * 
 * Serves as the top-level container for the QML UI shell.
 * Manages:
 * - Sidebar navigation (Phase 11.A)
 * - Page loading and transitions
 * - Theme application
 * - Shell state persistence
 * 
 * Phase 11 Status: Refactored to use new Sidebar component
 */

Rectangle {
    id: root
    color: Theme.background
    anchors.fill: parent
    
    // Width persistence for sidebar
    property int storedSidebarWidth: shellState ? shellState.sidebarWidth : 200
    
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

    Component.onCompleted: {
        console.log("[ShellRoot] Component loaded - Theme:", Theme.background)
        console.log("[ShellRoot] LauncherVM available:", !!ProjT.launcherVM)
        console.log("[ShellRoot] InstancesVM available:", !!ProjT.instancesVM)
        
        // Initialize instance list from backend
        if (ProjT.instancesVM) {
            console.log("[ShellRoot] Refreshing instances list...")
            ProjT.instancesVM.refreshInstances()
        }
    }

    Dialog {
        id: newInstanceDialog
        title: qsTr("New Instance")
        modal: true
        standardButtons: Dialog.Ok | Dialog.Cancel
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        width: 400

        ColumnLayout {
            anchors.fill: parent
            spacing: 10

            TextField {
                id: instanceNameField
                placeholderText: qsTr("Instance Name")
                Layout.fillWidth: true
                selectByMouse: true
            }

            TextField {
                id: instanceVersionField
                placeholderText: qsTr("Version (e.g. 1.20.1)")
                Layout.fillWidth: true
                selectByMouse: true
            }
        }

        onAccepted: {
            if (ProjT.instancesVM) {
                ProjT.instancesVM.createNewInstance(instanceNameField.text, instanceVersionField.text)
            }
            instanceNameField.text = ""
            instanceVersionField.text = ""
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0
        
        // === TOP BAR ===
        TopBar {
            id: topBarComponent
            Layout.fillWidth: true
            Layout.preferredHeight: 48
            
            onCreateNewInstance: {
                newInstanceDialog.open()
            }
            
            onOpenFolders: {
                if (ProjT.launcherVM) {
                    ProjT.launcherVM.openDataFolder()
                }
            }
            
            onOpenSettings: {
                if (ProjT.launcherVM) {
                    ProjT.launcherVM.setCurrentPage(LauncherViewModelEnums.Page.Settings)
                }
            }
            
            onOpenHelp: {
                if (ProjT.launcherVM) {
                    ProjT.launcherVM.openHelp()
                }
            }
            
            onCheckUpdates: {
                if (ProjT.launcherVM) {
                    ProjT.launcherVM.checkUpdates()
                }
            }
            
            onCatAction: {
                console.log("[ShellRoot] CAT action")
            }
            
            onAccountsMenu: {
                console.log("[ShellRoot] Accounts menu requested")
            }
        }

        // === MAIN CONTENT AREA (Sidebar + Page) ===
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            // === SIDEBAR (Phase 11.A) ===
            Sidebar {
                id: sidebarComponent
                Layout.preferredWidth: storedSidebarWidth
                Layout.fillHeight: true
                
                // Listen to page navigation requests
                onPageRequested: function(page) {
                    console.log("[ShellRoot] Page requested:", page)
                    pageLoader.source = pageSource(page)
                }
                
                // Listen to sidebar width changes for persistence
                onPreferredWidthChanged: {
                    Layout.preferredWidth = preferredWidth
                    if (shellState) {
                        shellState.sidebarWidth = preferredWidth
                    }
                }
            }

            // === PAGE CONTENT AREA ===
            Loader {
                id: pageLoader
                Layout.fillWidth: true
                Layout.fillHeight: true
                
                source: pageSource(ProjT.launcherVM ? 
                                   ProjT.launcherVM.currentPage : 
                                   LauncherViewModelEnums.Page.Instances)
                
                // Re-load when page changes
                Connections {
                    target: ProjT.launcherVM
                    function onCurrentPageChanged() {
                        pageLoader.source = pageSource(ProjT.launcherVM.currentPage)
                    }
                }
                
                // Page transition animation
                transitions: Transition {
                    NumberAnimation {
                        property: "opacity"
                        from: 0
                        to: 1
                        duration: 150
                        easing.type: Easing.OutQuad
                    }
                }
                
                asynchronous: true
            }
        }
        
        // === BOTTOM BAR ===
        BottomBar {
            id: bottomBarComponent
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            
            statusMessage: qsTr("Ready")
            
            onMoreNewsRequested: {
                if (ProjT.launcherVM) {
                    ProjT.launcherVM.setCurrentPage(LauncherViewModelEnums.Page.News)
                }
            }
        }
    }
}


