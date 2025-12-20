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
import "../Theme.js" as Theme

Rectangle {
    id: sidebar
    
    // Theme binding for reactive updates
    property var themeVM: ProjT.themeVM
    property int _themeUpdateCount: 0
    
    color: {
        var _ = _themeUpdateCount
        return themeVM ? Qt.darker(themeVM.windowColor, 1.05) : ThemeColors.toolBar
    }
    
    property color toolBarColor: {
        var _ = _themeUpdateCount
        return themeVM ? Qt.darker(themeVM.windowColor, 1.05) : ThemeColors.toolBar
    }
    property color borderColor: {
        var _ = _themeUpdateCount
        return themeVM ? Qt.darker(themeVM.windowColor, 1.2) : ThemeColors.border
    }
    property color textColor: {
        var _ = _themeUpdateCount
        return themeVM ? themeVM.textColor : ThemeColors.text
    }
    property color textSecondaryColor: {
        var _ = _themeUpdateCount
        return themeVM ? Qt.darker(themeVM.textColor, 1.3) : ThemeColors.textSecondary
    }
    property color highlightColor: {
        var _ = _themeUpdateCount
        return themeVM ? themeVM.highlightColor : ThemeColors.highlight
    }
    
    Connections {
        target: themeVM
        function onThemeColorsChanged() {
            sidebar._themeUpdateCount++
        }
    }
    
    signal pageRequested(int page)
    signal accountRequested(int accountIndex)
    signal themeToggleRequested()
    
    property int preferredWidth: 180
    property bool isCollapsed: false
    
    // Navigation entries - keep in sync with ShellRoot navEntries
    property var navEntries: [
        {
            title: qsTr("Instances"),
            page: LauncherViewModelEnums.Page.Instances,
            iconName: "minecraft",
            tooltip: qsTr("Manage your Minecraft instances")
        },
        {
            title: qsTr("News"),
            page: LauncherViewModelEnums.Page.News,
            iconName: "news",
            tooltip: qsTr("View latest launcher news")
        },
        {
            title: qsTr("Settings"),
            page: LauncherViewModelEnums.Page.Settings,
            iconName: "settings",
            tooltip: qsTr("Launcher and instance settings")
        },
        {
            title: qsTr("Logs"),
            page: LauncherViewModelEnums.Page.Logs,
            iconName: "log",
            tooltip: qsTr("View application and instance logs")
        },
        {
            title: qsTr("About"),
            page: LauncherViewModelEnums.Page.About,
            iconName: "about",
            tooltip: qsTr("About ProjT Launcher")
        }
    ]
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingM
        spacing: Theme.spacingS
        
        // === Header: Launcher Info ===
        Column {
            Layout.fillWidth: true
            spacing: Theme.spacingS
            
            Label {
                text: ProjT.launcherVM ? ProjT.launcherVM.displayName : qsTr("ProjT Launcher")
                color: ThemeColors.text
                font.pointSize: 14
                font.bold: true
                wrapMode: Text.WordWrap
                width: parent.width
            }
            
            Label {
                text: ProjT.launcherVM ? ProjT.launcherVM.versionString : ""
                color: ThemeColors.textSecondary
                font.pointSize: 11
                wrapMode: Text.WordWrap
                width: parent.width
            }
        }
        
        ToolSeparator { 
            Layout.fillWidth: true 
        }
        
        // === Navigation Buttons ===
        Column {
            Layout.fillWidth: true
            spacing: Theme.spacingS
            
            Repeater {
                model: navEntries
                delegate: Button {
                    id: navButton
                    checkable: true
                    
                    // Get current page from ViewModel
                    checked: ProjT.launcherVM && 
                            ProjT.launcherVM.currentPage === modelData.page
                    
                    implicitHeight: 40
                    Layout.fillWidth: true
                    
                    ToolTip.text: modelData.tooltip
                    ToolTip.visible: hovered && !isCollapsed
                    ToolTip.delay: 500
                    
                    // Background styling
                    background: Rectangle {
                        radius: Theme.radius
                        color: navButton.checked ? 
                                ThemeColors.highlight : 
                                (navButton.hovered ? ThemeColors.backgroundAlt : "transparent")
                        
                        border.color: navButton.checked ? ThemeColors.accent : ThemeColors.border
                        border.width: navButton.checked ? 1 : 0
                        
                        Behavior on color {
                            ColorAnimation { duration: 150 }
                        }
                    }
                    
                    // Content styling
                    contentItem: RowLayout {
                        anchors.fill: parent
                        anchors.margins: Theme.spacingS
                        spacing: Theme.spacingS

                        Image {
                            source: Theme.icon(modelData.iconName || "settings")
                            width: 18
                            height: 18
                            fillMode: Image.PreserveAspectFit
                            sourceSize.width: 18
                            sourceSize.height: 18
                            visible: source !== ""
                        }

                        Text {
                            text: modelData.title
                            color: navButton.checked ? ThemeColors.highlightedText : ThemeColors.text
                            font.pointSize: 12
                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                            Layout.fillWidth: true

                            Behavior on color {
                                ColorAnimation { duration: 150 }
                            }
                        }
                    }
                    
                    onClicked: {
                        console.log("Navigation clicked:", modelData.title, "->", modelData.page)
                        if (ProjT.launcherVM) {
                            ProjT.launcherVM.currentPage = modelData.page
                        }
                        sidebar.pageRequested(modelData.page)
                    }
                }
            }
        }
        
        // === Spacer ===
        Item { Layout.fillHeight: true }
        
        // === Bottom Actions ===
        Column {
            Layout.fillWidth: true
            spacing: Theme.spacingS
            
            ToolSeparator { 
                Layout.fillWidth: true 
            }
            
            // Account Selector
            Button {
                id: accountButton
                text: ProjT.accountsVM && ProjT.accountsVM.defaultAccountName ? 
                      ProjT.accountsVM.defaultAccountName : 
                      qsTr("No Account")
                
                implicitHeight: 40
                Layout.fillWidth: true
                
                checkable: false
                
                background: Rectangle {
                    radius: Theme.radius
                    color: accountButton.hovered ? ThemeColors.backgroundAlt : ThemeColors.surface
                    border.color: ThemeColors.border
                    border.width: 1
                    
                    Behavior on color {
                        ColorAnimation { duration: 150 }
                    }
                }
                
                contentItem: RowLayout {
                    anchors.fill: parent
                    anchors.margins: Theme.spacingS
                    spacing: Theme.spacingS
                    
                    Rectangle {
                        width: 32
                        height: 32
                        radius: 4
                        color: ThemeColors.backgroundAlt
                        Layout.alignment: Qt.AlignVCenter
                        
                        Text {
                            anchors.centerIn: parent
                            text: "👤"
                            font.pointSize: 16
                        }
                    }
                    
                    Column {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignVCenter
                        spacing: 2
                        
                        Text {
                            text: qsTr("Account")
                            color: ThemeColors.textSecondary
                            font.pointSize: 9
                            font.italic: true
                        }
                        
                        Text {
                            text: accountButton.text
                            color: ThemeColors.text
                            font.pointSize: 11
                            elide: Text.ElideRight
                            width: parent.width
                        }
                    }
                    
                    Text {
                        text: "▼"
                        color: ThemeColors.textSecondary
                        font.pointSize: 8
                        Layout.alignment: Qt.AlignVCenter
                        opacity: 0.6
                    }
                }
                
                onClicked: accountMenu.open()
                
                Menu {
                    id: accountMenu
                    y: accountButton.height
                    
                    // Dynamic account list from AccountsViewModel
                    Repeater {
                        model: ProjT.accountsVM ? ProjT.accountsVM.model : null
                        
                        MenuItem {
                            text: model.name || model.display || ""
                            checkable: true
                            checked: ProjT.accountsVM && 
                                     ProjT.accountsVM.defaultAccountIndex === index
                            
                            onTriggered: {
                                if (ProjT.accountsVM) {
                                    ProjT.accountsVM.setDefaultAccount(index)
                                    sidebar.accountRequested(index)
                                }
                            }
                        }
                    }
                    
                    MenuSeparator { 
                        visible: ProjT.accountsVM && ProjT.accountsVM.count > 0
                    }
                    
                    MenuItem {
                        text: qsTr("Add Microsoft Account...")
                        onTriggered: {
                            if (ProjT.accountsVM) {
                                ProjT.accountsVM.addMicrosoftAccount()
                            }
                        }
                    }
                    
                    MenuItem {
                        text: qsTr("Add Offline Account...")
                        onTriggered: {
                            offlineAccountDialog.open()
                        }
                    }
                    
                    MenuSeparator { }
                    
                    MenuItem {
                        text: qsTr("Manage Accounts...")
                        onTriggered: {
                            console.log("Account management requested")
                            // Navigate to accounts settings page
                            if (ProjT.launcherVM) {
                                ProjT.launcherVM.currentPage = LauncherViewModelEnums.Page.Settings
                            }
                            sidebar.pageRequested(LauncherViewModelEnums.Page.Settings)
                        }
                    }
                    
                    MenuItem {
                        text: qsTr("Refresh All Accounts")
                        enabled: ProjT.accountsVM && ProjT.accountsVM.count > 0
                        onTriggered: {
                            if (ProjT.accountsVM) {
                                ProjT.accountsVM.refreshAllAccounts()
                            }
                        }
                    }
                }
            }
            
            // Offline Account Dialog
            Dialog {
                id: offlineAccountDialog
                title: qsTr("Add Offline Account")
                standardButtons: Dialog.Ok | Dialog.Cancel
                modal: true
                anchors.centerIn: Overlay.overlay
                width: 300
                
                ColumnLayout {
                    anchors.fill: parent
                    spacing: Theme.spacingM
                    
                    Label {
                        text: qsTr("Username:")
                        color: ThemeColors.text
                    }
                    
                    TextField {
                        id: offlineUsernameField
                        Layout.fillWidth: true
                        placeholderText: qsTr("Enter username...")
                    }
                }
                
                onAccepted: {
                    if (offlineUsernameField.text.trim() !== "" && ProjT.accountsVM) {
                        ProjT.accountsVM.addOfflineAccount(offlineUsernameField.text.trim())
                        offlineUsernameField.text = ""
                    }
                }
                
                onRejected: {
                    offlineUsernameField.text = ""
                }
            }
            
            // MS Login URL Dialog
            Dialog {
                id: msLoginUrlDialog
                title: qsTr("Microsoft Login")
                standardButtons: Dialog.Close
                modal: true
                anchors.centerIn: Overlay.overlay
                width: 400
                
                property string loginUrl: ""
                property string deviceCode: ""
                
                ColumnLayout {
                    anchors.fill: parent
                    spacing: Theme.spacingM
                    
                    Label {
                        text: qsTr("Please open the following URL in your browser and enter the code:")
                        color: ThemeColors.text
                        wrapMode: Text.WordWrap
                        Layout.fillWidth: true
                    }
                    
                    TextField {
                        id: urlField
                        text: msLoginUrlDialog.loginUrl
                        readOnly: true
                        selectByMouse: true
                        Layout.fillWidth: true
                    }
                    
                    Label {
                        text: qsTr("Code: ") + msLoginUrlDialog.deviceCode
                        font.bold: true
                        font.pointSize: 14
                        color: ThemeColors.accent
                    }
                    
                    Button {
                        text: qsTr("Copy URL")
                        onClicked: {
                            urlField.selectAll()
                            urlField.copy()
                        }
                    }
                    
                    BusyIndicator {
                        Layout.alignment: Qt.AlignHCenter
                        running: msLoginUrlDialog.visible
                    }
                    
                    Label {
                        text: qsTr("Waiting for authentication...")
                        color: ThemeColors.textSecondary
                        Layout.alignment: Qt.AlignHCenter
                    }
                }
            }
            
            // Connect to AccountsViewModel signals
            Connections {
                target: ProjT.accountsVM
                
                function onLoginUrlReady(url, code) {
                    msLoginUrlDialog.loginUrl = url
                    msLoginUrlDialog.deviceCode = code || ""
                    msLoginUrlDialog.open()
                }
                
                function onLoginFinished(success) {
                    msLoginUrlDialog.close()
                    if (success) {
                        console.log("Login successful")
                    }
                }
                
                function onLoginFailed(error) {
                    msLoginUrlDialog.close()
                    console.log("Login failed:", error)
                }
            }
            }
            
            // Theme Toggle
            Button {
                id: themeButton
                text: qsTr("Toggle Theme")
                
                implicitHeight: 40
                Layout.fillWidth: true
                
                background: Rectangle {
                    radius: Theme.radius
                    color: themeButton.hovered ? ThemeColors.backgroundAlt : ThemeColors.surface
                    border.color: ThemeColors.border
                    border.width: 1
                    
                    Behavior on color {
                        ColorAnimation { duration: 150 }
                    }
                }
                
                contentItem: RowLayout {
                    anchors.fill: parent
                    anchors.margins: Theme.spacingS
                    spacing: Theme.spacingS
                    
                    Text {
                        text: "🌙"  // Moon/sun emoji
                        font.pointSize: 14
                        Layout.alignment: Qt.AlignVCenter
                    }
                    
                    Text {
                        text: themeButton.text
                        color: ThemeColors.text
                        font.pointSize: 11
                        Layout.fillWidth: true
                    }
                }
                
                onClicked: {
                    console.log("Theme toggle requested")
                    sidebar.themeToggleRequested()
                }
            }
        }
    }
    
    // === Resize Handle ===
    MouseArea {
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 5
        cursorShape: Qt.SizeHorCursor
        
        property real startX: 0
        property real startWidth: 0
        
        onPressed: function(mouse) {
            startX = mouse.x
            startWidth = sidebar.width
        }
        
        onPositionChanged: function(mouse) {
            if (pressed) {
                var delta = mouse.x - startX
                var newWidth = Math.max(150, Math.min(400, startWidth + delta))
                sidebar.preferredWidth = newWidth
                
                // Persist to settings
                if (shellState) {
                    shellState.sidebarWidth = newWidth
                }
            }
        }
    }
}
