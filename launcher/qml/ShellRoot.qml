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
    
    // Instance toolbar visibility (only on Instances page)
    // Main window always shows Instances - other pages open as dialogs/windows
    readonly property bool isInstancesPage: true
    
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
    
    // === New Instance Window (like Qt Widget NewInstanceDialog) ===
    // Opens as separate window with PageContainer style layout
    function showNewInstanceWindow() { newInstanceWindowLoader.active = true }
    
    Loader {
        id: newInstanceWindowLoader
        active: false
        sourceComponent: Window {
            id: newInstanceWindow
            title: qsTr("New Instance")
            width: 730
            height: 600
            minimumWidth: 600
            minimumHeight: 450
            color: Theme.background
            flags: Qt.Window
            visible: true
            
            onClosing: newInstanceWindowLoader.active = false
            
            property string selectedIconKey: "default"
            property int currentPageIndex: 0
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 0
                spacing: 0
                
                // === Top Section: Icon + Name + Group (like Qt UI) ===
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 100
                    color: Theme.surfaceBackground
                    
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: Theme.spacingM
                        spacing: Theme.spacingM
                        
                        // Icon Button (80x80 like Qt UI)
                        Button {
                            id: iconButton
                            Layout.preferredWidth: 80
                            Layout.preferredHeight: 80
                            
                            Image {
                                anchors.centerIn: parent
                                width: 64
                                height: 64
                                source: Theme.instanceIconFromKey(newInstanceWindow.selectedIconKey)
                                fillMode: Image.PreserveAspectFit
                            }
                            
                            onClicked: {
                                // TODO: Icon picker dialog
                                console.log("Icon picker requested")
                            }
                            
                            ToolTip.text: qsTr("Click to change icon")
                            ToolTip.visible: hovered
                        }
                        
                        // Name and Group fields
                        GridLayout {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            columns: 2
                            rowSpacing: Theme.spacingS
                            columnSpacing: Theme.spacingM
                            
                            Label {
                                text: qsTr("&Name:")
                                color: Theme.textPrimary
                            }
                            
                            TextField {
                                id: instNameField
                                Layout.fillWidth: true
                                placeholderText: qsTr("My Instance")
                                selectByMouse: true
                                maximumLength: 128
                            }
                            
                            Label {
                                text: qsTr("&Group:")
                                color: Theme.textPrimary
                            }
                            
                            ComboBox {
                                id: groupCombo
                                Layout.fillWidth: true
                                editable: true
                                model: {
                                    var groups = ProjT.instancesVM ? ProjT.instancesVM.groupList : null
                                    if (groups && groups.length > 0) {
                                        return groups
                                    }
                                    return [""]
                                }
                                
                                Component.onCompleted: {
                                    // Insert empty option for "No group"
                                    if (model && model.length === 0) {
                                        // handled by model
                                    }
                                }
                            }
                        }
                    }
                }
                
                // Separator
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 1
                    color: Theme.border
                }
                
                // === Page Container Area ===
                RowLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    spacing: 0
                    
                    // Left sidebar - Page list
                    Rectangle {
                        Layout.preferredWidth: 150
                        Layout.fillHeight: true
                        color: Theme.surfaceBackground
                        
                        ListView {
                            id: pageList
                            anchors.fill: parent
                            anchors.margins: Theme.spacingS
                            spacing: 2
                            currentIndex: newInstanceWindow.currentPageIndex
                            
                            model: ListModel {
                                ListElement { name: "Custom"; icon: "minecraft"; platform: "" }
                                ListElement { name: "Import"; icon: "viewfolder"; platform: "" }
                                ListElement { name: "ATLauncher"; icon: "gear"; platform: "atlauncher" }
                                ListElement { name: "CurseForge"; icon: "flame"; platform: "curseforge" }
                                ListElement { name: "FTB Legacy"; icon: "ftb_logo"; platform: "ftb" }
                                ListElement { name: "FTB App"; icon: "ftb_logo"; platform: "ftb" }
                                ListElement { name: "Modrinth"; icon: "modrinth"; platform: "modrinth" }
                                ListElement { name: "Technic"; icon: "brick"; platform: "technic" }
                            }
                            
                            delegate: ItemDelegate {
                                width: pageList.width
                                height: 36
                                highlighted: pageList.currentIndex === index
                                
                                contentItem: RowLayout {
                                    spacing: Theme.spacingS
                                    
                                    Image {
                                        Layout.preferredWidth: 20
                                        Layout.preferredHeight: 20
                                        source: model.platform ? Theme.platformIcon(model.platform) : 
                                                (model.icon === "viewfolder" ? Theme.icon("viewfolder") : 
                                                 "qrc:/icons/multimc/scalable/instances/" + model.icon + ".svg")
                                        fillMode: Image.PreserveAspectFit
                                    }
                                    
                                    Label {
                                        text: model.name
                                        color: Theme.textPrimary
                                        Layout.fillWidth: true
                                    }
                                }
                                
                                onClicked: {
                                    pageList.currentIndex = index
                                    newInstanceWindow.currentPageIndex = index
                                }
                            }
                        }
                    }
                    
                    // Separator
                    Rectangle {
                        Layout.preferredWidth: 1
                        Layout.fillHeight: true
                        color: Theme.border
                    }
                    
                    // Right content area - Page stack
                    StackLayout {
                        id: pageStack
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        currentIndex: newInstanceWindow.currentPageIndex
                        
                        // Custom page
                        NewInstanceCustomPage {
                            id: customPage
                        }
                        
                        // Import page
                        NewInstanceImportPage {
                            id: importPage
                        }
                        
                        // ATLauncher placeholder
                        NewInstancePlaceholderPage {
                            pageName: "ATLauncher"
                        }
                        
                        // CurseForge placeholder
                        NewInstancePlaceholderPage {
                            pageName: "CurseForge"
                        }
                        
                        // FTB Legacy placeholder
                        NewInstancePlaceholderPage {
                            pageName: "FTB Legacy"
                        }
                        
                        // FTB App placeholder
                        NewInstancePlaceholderPage {
                            pageName: "FTB App"
                        }
                        
                        // Modrinth placeholder
                        NewInstancePlaceholderPage {
                            pageName: "Modrinth"
                        }
                        
                        // Technic placeholder
                        NewInstancePlaceholderPage {
                            pageName: "Technic"
                        }
                    }
                }
                
                // Separator
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 1
                    color: Theme.border
                }
                
                // === Bottom buttons ===
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 50
                    color: Theme.surfaceBackground
                    
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: Theme.spacingM
                        spacing: Theme.spacingM
                        
                        Button {
                            text: qsTr("Help")
                            icon.name: "help-contents"
                            onClicked: {
                                // Open help URL
                                Qt.openUrlExternally("https://prismlauncher.org/wiki/getting-started/create-instance/")
                            }
                        }
                        
                        Item { Layout.fillWidth: true }
                        
                        Button {
                            text: qsTr("OK")
                            enabled: instNameField.text.length > 0
                            onClicked: {
                                if (ProjT.instancesVM && instNameField.text.length > 0) {
                                    var version = customPage.selectedVersion || ""
                                    var group = groupCombo.currentText || ""
                                    ProjT.instancesVM.createNewInstance(instNameField.text, version, group)
                                }
                                newInstanceWindow.close()
                            }
                        }
                        
                        Button {
                            text: qsTr("Cancel")
                            onClicked: newInstanceWindow.close()
                        }
                    }
                }
            }
        }
    }
    
    // === Rename Dialog ===
    Dialog {
        id: renameDialog
        title: qsTr("Rename Instance")
        modal: true
        standardButtons: Dialog.Ok | Dialog.Cancel
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        width: 360
        
        property string currentName: ""
        
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
                text: renameDialog.currentName
                selectByMouse: true
            }
        }
        
        onAccepted: {
            if (ProjT.instancesVM && renameField.text.length > 0) {
                ProjT.instancesVM.renameSelectedInstance(renameField.text)
            }
        }
        
        onOpened: {
            currentName = ProjT.instancesVM ? ProjT.instancesVM.selectedInstanceName : ""
            renameField.text = currentName
            renameField.selectAll()
        }
    }
    
    // === Duplicate Dialog ===
    Dialog {
        id: duplicateDialog
        title: qsTr("Copy Instance")
        modal: true
        standardButtons: Dialog.Ok | Dialog.Cancel
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        width: 360
        
        property string currentName: ""
        
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
            currentName = ProjT.instancesVM ? ProjT.instancesVM.selectedInstanceName : ""
            duplicateNameField.text = currentName + qsTr(" Copy")
            duplicateNameField.selectAll()
        }
    }
    
    // === Delete Confirmation Dialog ===
    Dialog {
        id: deleteDialog
        title: qsTr("Delete Instance")
        modal: true
        standardButtons: Dialog.Yes | Dialog.No
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        width: 380
        
        property string currentName: ""
        
        onOpened: {
            currentName = ProjT.instancesVM ? ProjT.instancesVM.selectedInstanceName : ""
        }
        
        Label {
            text: qsTr("Delete \"%1\"?\n\nThis action cannot be undone.").arg(deleteDialog.currentName)
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
    
    // === Change Group Dialog ===
    Dialog {
        id: changeGroupDialog
        title: qsTr("Change Group")
        modal: true
        standardButtons: Dialog.Ok | Dialog.Cancel
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        width: 360
        
        ColumnLayout {
            anchors.fill: parent
            spacing: 10
            
            Label {
                text: qsTr("Group name:")
                color: Theme.textPrimary
            }
            
            TextField {
                id: groupNameField
                Layout.fillWidth: true
                placeholderText: qsTr("Leave empty for no group")
                selectByMouse: true
            }
        }
        
        onAccepted: {
            if (ProjT.instancesVM) {
                ProjT.instancesVM.setSelectedGroup(groupNameField.text)
            }
        }
        
        onOpened: {
            groupNameField.text = ""
        }
    }

    // === MAIN LAYOUT (matches Qt Widget MainWindow) ===
    ColumnLayout {
        anchors.fill: parent
        spacing: 0
        
        // ════════════════════════════════════════════════════════════
        // MAIN TOOLBAR (top horizontal toolbar)
        // ════════════════════════════════════════════════════════════
        MainToolBar {
            id: mainToolBar
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            
            onAddInstance: showNewInstanceWindow()
            
            onShowSettings: {
                showSettingsWindow()
            }
            
            onShowAbout: {
                showAboutWindow()
            }
            
            onShowLogs: {
                showLogsWindow()
            }
            
            onCheckUpdate: {
                if (ProjT.launcherVM) {
                    ProjT.launcherVM.checkUpdates()
                }
            }
            
            onAccountsMenuRequested: {
                console.log("[ShellRoot] Accounts menu requested")
                showAccountsWindow()
            }
            
            // Folder actions (using DesktopServices)
            onOpenLauncherFolder: {
                if (ProjT.launcherVM) {
                    ProjT.launcherVM.openLauncherFolder()
                }
            }
            
            onOpenInstancesFolder: {
                if (ProjT.launcherVM) {
                    ProjT.launcherVM.openInstancesFolder()
                }
            }
            
            onOpenModsFolder: {
                if (ProjT.launcherVM) {
                    ProjT.launcherVM.openModsFolder()
                }
            }
            
            onOpenSkinsFolder: {
                if (ProjT.launcherVM) {
                    ProjT.launcherVM.openSkinsFolder()
                }
            }
        }
        
        // ════════════════════════════════════════════════════════════
        // CENTRAL AREA (Page Content + Instance Toolbar)
        // ════════════════════════════════════════════════════════════
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0
            
            // === INSTANCE PAGE (always visible - main content) ===
            InstancePage {
                id: instancePage
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
            
            // === Separator Line ===
            Rectangle {
                visible: isInstancesPage
                Layout.preferredWidth: 1
                Layout.fillHeight: true
                color: "#323742"
            }
            
            // === INSTANCE TOOLBAR (right vertical toolbar) ===
            InstanceToolBar {
                id: instanceToolBar
                visible: isInstancesPage
                Layout.preferredWidth: 110
                Layout.fillHeight: true
                
                onEditInstance: {
                    console.log("[ShellRoot] Edit instance requested")
                    if (ProjT.instancesVM) {
                        ProjT.instancesVM.openInstanceSettings()
                    }
                }
                
                onChangeGroup: changeGroupDialog.open()
                
                onExportInstance: {
                    console.log("[ShellRoot] Export instance requested")
                    if (ProjT.instancesVM) {
                        ProjT.instancesVM.exportSelectedInstance()
                    }
                }
                
                onManageBackups: {
                    console.log("[ShellRoot] Manage backups requested")
                    if (ProjT.instancesVM) {
                        ProjT.instancesVM.manageSelectedBackups()
                    }
                }
                
                onCopyInstance: duplicateDialog.open()
                
                onDeleteInstance: deleteDialog.open()
                
                onCreateShortcut: {
                    console.log("[ShellRoot] Create shortcut requested")
                    if (ProjT.instancesVM) {
                        ProjT.instancesVM.createSelectedShortcut()
                    }
                }
            }
        }
        
        // ════════════════════════════════════════════════════════════
        // NEWS TOOLBAR (bottom horizontal toolbar)
        // ════════════════════════════════════════════════════════════
        NewsToolBar {
            id: newsToolBar
            Layout.fillWidth: true
            Layout.preferredHeight: 28
            
            onMoreNewsClicked: {
                showNewsWindow()
            }
            
            onNewsClicked: {
                showNewsWindow()
            }
        }
        
        // ════════════════════════════════════════════════════════════
        // STATUS BAR (bottom status bar)
        // ════════════════════════════════════════════════════════════
        StatusBar {
            id: statusBar
            Layout.fillWidth: true
            Layout.preferredHeight: 24
        }
    }
    
    // ════════════════════════════════════════════════════════════════
    // SEPARATE WINDOWS FOR SETTINGS / ABOUT / LOGS / NEWS / ACCOUNTS
    // ════════════════════════════════════════════════════════════════
    
    // Helper function to show windows
    function showSettingsWindow() { settingsWindowLoader.active = true }
    function showAboutWindow() { aboutWindowLoader.active = true }
    function showLogsWindow() { logsWindowLoader.active = true }
    function showNewsWindow() { newsWindowLoader.active = true }
    function showAccountsWindow() { accountsWindowLoader.active = true }
    
    // === Settings Window (PageContainer style like Qt Widget) ===
    Loader {
        id: settingsWindowLoader
        active: false
        sourceComponent: Window {
            id: settingsWindow
            title: qsTr("Settings")
            width: 900
            height: 650
            minimumWidth: 700
            minimumHeight: 500
            color: Theme.background
            flags: Qt.Window
            visible: true
            
            onClosing: settingsWindowLoader.active = false
            
            property int currentPageIndex: 0
            
            ColumnLayout {
                anchors.fill: parent
                spacing: 0
                
                // === Page Container Area ===
                RowLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    spacing: 0
                    
                    // Left sidebar - Category list
                    Rectangle {
                        Layout.preferredWidth: 180
                        Layout.fillHeight: true
                        color: Theme.surfaceBackground
                        
                        ListView {
                            id: settingsPageList
                            anchors.fill: parent
                            anchors.margins: Theme.spacingS
                            spacing: 2
                            currentIndex: settingsWindow.currentPageIndex
                            
                            model: ListModel {
                                ListElement { name: "Launcher"; iconName: "settings" }
                                ListElement { name: "Appearance"; iconName: "appearance" }
                                ListElement { name: "Minecraft"; iconName: "minecraft" }
                                ListElement { name: "Java"; iconName: "java" }
                                ListElement { name: "Language"; iconName: "language" }
                                ListElement { name: "External Tools"; iconName: "externaltools" }
                                ListElement { name: "Accounts"; iconName: "accounts" }
                                ListElement { name: "API"; iconName: "news" }
                                ListElement { name: "Proxy"; iconName: "proxy" }
                            }
                            
                            delegate: ItemDelegate {
                                width: settingsPageList.width
                                height: 40
                                highlighted: settingsPageList.currentIndex === index
                                
                                icon.name: model.iconName
                                icon.width: 24
                                icon.height: 24
                                
                                text: model.name
                                
                                onClicked: {
                                    settingsPageList.currentIndex = index
                                    settingsWindow.currentPageIndex = index
                                }
                            }
                        }
                    }
                    
                    // Separator
                    Rectangle {
                        Layout.preferredWidth: 1
                        Layout.fillHeight: true
                        color: Theme.border
                    }
                    
                    // Right content area
                    ColumnLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        spacing: 0
                        
                        // Header with page title
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 40
                            color: Theme.surfaceBackground
                            
                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: Theme.spacingM
                                anchors.rightMargin: Theme.spacingM
                                
                                Label {
                                    text: settingsPageList.model.get(settingsWindow.currentPageIndex).name
                                    font.pointSize: 12
                                    font.bold: true
                                    color: Theme.textPrimary
                                }
                                
                                Item { Layout.fillWidth: true }
                            }
                        }
                        
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 1
                            color: Theme.border
                        }
                        
                        // Page stack
                        StackLayout {
                            id: settingsPageStack
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            currentIndex: settingsWindow.currentPageIndex
                            
                            // Launcher settings
                            SettingsLauncherPage {}
                            
                            // Appearance settings
                            SettingsAppearancePage {}
                            
                            // Minecraft settings
                            SettingsMinecraftPage {}
                            
                            // Java settings
                            SettingsJavaPage {}
                            
                            // Language settings
                            SettingsLanguagePage {}
                            
                            // External Tools settings
                            SettingsExternalToolsPage {}
                            
                            // Accounts settings
                            SettingsAccountsPage {}
                            
                            // API settings
                            SettingsAPIPage {}
                            
                            // Proxy settings
                            SettingsProxyPage {}
                        }
                    }
                }
                
                // Separator
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 1
                    color: Theme.border
                }
                
                // === Bottom buttons ===
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 50
                    color: Theme.surfaceBackground
                    
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: Theme.spacingM
                        spacing: Theme.spacingM
                        
                        Button {
                            text: qsTr("Help")
                            icon.name: "help-contents"
                            onClicked: {
                                Qt.openUrlExternally("https://prismlauncher.org/wiki/")
                            }
                        }
                        
                        Item { Layout.fillWidth: true }
                        
                        Button {
                            text: qsTr("OK")
                            onClicked: {
                                // Apply settings and close
                                if (ProjT.settingsVM) {
                                    ProjT.settingsVM.applyChanges()
                                }
                                settingsWindow.close()
                            }
                        }
                        
                        Button {
                            text: qsTr("Cancel")
                            onClicked: settingsWindow.close()
                        }
                    }
                }
            }
        }
    }
    
    // === About Window ===
    Loader {
        id: aboutWindowLoader
        active: false
        sourceComponent: Window {
            id: aboutWindow
            title: qsTr("About %1").arg(ProjT.launcherVM ? ProjT.launcherVM.displayName : "ProjT Launcher")
            width: 600
            height: 500
            minimumWidth: 400
            minimumHeight: 350
            color: Theme.background
            flags: Qt.Window
            visible: true
            
            onClosing: aboutWindowLoader.active = false
            
            AboutPage {
                anchors.fill: parent
                anchors.margins: Theme.spacingM
            }
        }
    }
    
    // === Logs Window ===
    Loader {
        id: logsWindowLoader
        active: false
        sourceComponent: Window {
            id: logsWindow
            title: qsTr("Logs")
            width: 900
            height: 650
            minimumWidth: 600
            minimumHeight: 400
            color: Theme.background
            flags: Qt.Window
            visible: true
            
            onClosing: logsWindowLoader.active = false
            
            LogsPage {
                anchors.fill: parent
                anchors.margins: Theme.spacingM
            }
        }
    }
    
    // === News Window ===
    Loader {
        id: newsWindowLoader
        active: false
        sourceComponent: Window {
            id: newsWindow
            title: qsTr("News")
            width: 800
            height: 600
            minimumWidth: 500
            minimumHeight: 400
            color: Theme.background
            flags: Qt.Window
            visible: true
            
            onClosing: newsWindowLoader.active = false
            
            NewsPage {
                anchors.fill: parent
                anchors.margins: Theme.spacingM
            }
        }
    }
    
    // === Accounts Window ===
    Loader {
        id: accountsWindowLoader
        active: false
        sourceComponent: Window {
            id: accountsWindow
            title: qsTr("Manage Accounts")
            width: 550
            height: 450
            minimumWidth: 400
            minimumHeight: 350
            color: Theme.background
            flags: Qt.Window
            visible: true
            
            onClosing: accountsWindowLoader.active = false
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: Theme.spacingM
                spacing: Theme.spacingM
                
                Label {
                    text: qsTr("Account Management")
                    font.pointSize: 14
                    font.bold: true
                    color: Theme.textPrimary
                }
                
                Label {
                    text: qsTr("Manage your Minecraft accounts below.")
                    color: Theme.textSecondary
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }
                
                // Account list placeholder
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: Theme.surfaceBackground
                    radius: Theme.radiusS
                    border.color: Theme.border
                    border.width: 1
                    
                    Label {
                        anchors.centerIn: parent
                        text: qsTr("No accounts added yet")
                        color: Theme.textSecondary
                    }
                }
                
                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingM
                    
                    Button {
                        text: qsTr("Add Microsoft Account")
                        icon.name: "list-add"
                        onClicked: {
                            if (ProjT.launcherVM) {
                                ProjT.launcherVM.openAccountsManager()
                            }
                        }
                    }
                    
                    Item { Layout.fillWidth: true }
                    
                    Button {
                        text: qsTr("Close")
                        onClicked: accountsWindow.close()
                    }
                }
            }
        }
    }
}