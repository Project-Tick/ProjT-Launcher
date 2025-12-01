// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import ProjTLauncher 1.0
import "../Theme.js" as Theme

/**
 * Sidebar Navigation Component
 * 
 * Provides primary navigation between main application pages.
 * Features:
 * - Navigation buttons (Instances, News, Settings, Logs, About)
 * - Launcher info display (name, version)
 * - Account selector
 * - Theme toggle
 * - Resizable width with persistence
 * 
 * Signals:
 * - pageRequested(int): Emitted when user clicks a nav button
 * - accountRequested(int): Emitted when user selects different account
 * - themeRequested(): Emitted when user toggles theme
 */

Rectangle {
    id: sidebar
    color: Theme.surface
    
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
            icon: ":/icons/instances.png",
            tooltip: qsTr("Manage your Minecraft instances")
        },
        { 
            title: qsTr("News"), 
            page: LauncherViewModelEnums.Page.News,
            icon: ":/icons/news.png",
            tooltip: qsTr("View latest launcher news")
        },
        { 
            title: qsTr("Settings"), 
            page: LauncherViewModelEnums.Page.Settings,
            icon: ":/icons/settings.png",
            tooltip: qsTr("Launcher and instance settings")
        },
        { 
            title: qsTr("Logs"), 
            page: LauncherViewModelEnums.Page.Logs,
            icon: ":/icons/logs.png",
            tooltip: qsTr("View application and instance logs")
        },
        { 
            title: qsTr("About"), 
            page: LauncherViewModelEnums.Page.About,
            icon: ":/icons/about.png",
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
                color: Theme.textPrimary
                font.pointSize: 14
                font.bold: true
                wrapMode: Text.WordWrap
                width: parent.width
            }
            
            Label {
                text: ProjT.launcherVM ? ProjT.launcherVM.versionString : ""
                color: Theme.textSecondary
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
                    text: modelData.title
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
                                "#2c3440" : 
                                (navButton.hovered ? "#2a2d33" : "transparent")
                        
                        border.color: navButton.checked ? Theme.accent : "#323742"
                        border.width: navButton.checked ? 1 : 0
                        
                        Behavior on color {
                            ColorAnimation { duration: 150 }
                        }
                    }
                    
                    // Content styling
                    contentItem: Text {
                        text: navButton.text
                        anchors.centerIn: parent
                        color: navButton.checked ? "#e6f0ff" : Theme.textPrimary
                        font.pointSize: 12
                        
                        Behavior on color {
                            ColorAnimation { duration: 150 }
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
                text: ProjT.accountsVM && ProjT.accountsVM.currentAccountName ? 
                      ProjT.accountsVM.currentAccountName : 
                      qsTr("No Account")
                
                implicitHeight: 40
                Layout.fillWidth: true
                
                checkable: false
                
                background: Rectangle {
                    radius: Theme.radius
                    color: accountButton.hovered ? "#2a2d33" : "#1e2227"
                    border.color: "#323742"
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
                        color: "#3d4d60"
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
                            color: Theme.textSecondary
                            font.pointSize: 9
                            font.italic: true
                        }
                        
                        Text {
                            text: accountButton.text
                            color: Theme.textPrimary
                            font.pointSize: 11
                            elide: Text.ElideRight
                            width: parent.width
                        }
                    }
                    
                    Text {
                        text: "▼"
                        color: Theme.textSecondary
                        font.pointSize: 8
                        Layout.alignment: Qt.AlignVCenter
                        opacity: 0.6
                    }
                }
                
                onClicked: accountMenu.open()
                
                Menu {
                    id: accountMenu
                    y: accountButton.height
                    
                    // TODO: Populate from AccountsViewModel when available
                    // For now, show static account management option
                    
                    // Placeholder: In future, this will be populated dynamically:
                    // Repeater {
                    //     model: accountsVM ? accountsVM.accounts : []
                    //     MenuItem {
                    //         text: modelData.name
                    //         onTriggered: accountsVM.selectAccount(modelData.id)
                    //     }
                    // }
                    
                    MenuItem {
                        text: qsTr("Manage Accounts...")
                        onTriggered: {
                            console.log("Account management requested")
                            // Will open settings dialog or full accounts page
                        }
                    }
                    
                    MenuSeparator { }
                    
                    MenuItem {
                        text: qsTr("Logout")
                        onTriggered: {
                            console.log("Logout requested")
                        }
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
                    color: themeButton.hovered ? "#2a2d33" : "#1e2227"
                    border.color: "#323742"
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
                        color: Theme.textPrimary
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
