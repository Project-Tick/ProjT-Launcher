// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import ProjTLauncher 1.0
import "components"
import "dialogs"
import "instance"
import "modplatform"
import "wizard"
import "Theme.js" as Theme

Rectangle {
    id: root
    anchors.fill: parent

    // Theme binding - directly from themeVM for reliable updates
    property var themeVM: ProjT.themeVM
    property int _themeUpdateCount: 0

    // Root background color with direct theme binding
    color: {
        var _ = _themeUpdateCount;
        return themeVM ? themeVM.windowColor : ThemeColors.background;
    }

    // Instance toolbar visibility (only on Instances page)
    // Main window always shows Instances - other pages open as dialogs/windows
    readonly property bool isInstancesPage: true

    // Theme change connection - forces ThemeColors singleton and root to update
    Connections {
        target: themeVM
        function onThemeColorsChanged() {
            console.log("[ShellRoot] Theme colors changed - updating UI");
            ThemeColors.forceUpdate();
            root._themeUpdateCount++;
        }
    }

    Component.onCompleted: {
        console.log("[ShellRoot] Component loaded");
        console.log("[ShellRoot] LauncherVM available:", !!ProjT.launcherVM);
        console.log("[ShellRoot] InstancesVM available:", !!ProjT.instancesVM);
        console.log("[ShellRoot] ThemeVM available:", !!ProjT.themeVM);

        // Initialize theme colors
        if (themeVM) {
            ThemeColors.forceUpdate();
            _themeUpdateCount++;
        }

        // Initialize instance list from backend
        if (ProjT.instancesVM) {
            console.log("[ShellRoot] Refreshing instances list...");
            ProjT.instancesVM.refreshInstances();
        }
    }

    // === New Instance Window (like Qt Widget NewInstanceDialog) ===
    // Opens as separate window with PageContainer style layout
    function showNewInstanceWindow() {
        newInstanceWindowLoader.active = true;
    }
    function openNewInstanceDialog(initialGroup, importUrl) {
        newInstanceWindowLoader.active = true;
        Qt.callLater(function () {
            if (newInstanceWindowLoader.item && newInstanceWindowLoader.item.openWithParams) {
                newInstanceWindowLoader.item.openWithParams(initialGroup || "", importUrl || "");
            }
        });
    }
    function openSettingsPage(pageKey) {
        settingsWindowLoader.active = true;
        Qt.callLater(function () {
            if (!settingsWindowLoader.item)
                return;
            var index = 0;
            switch (pageKey) {
            case "global-settings":
                index = 0;
                break;
            case "appearance":
                index = 1;
                break;
            case "minecraft-settings":
                index = 2;
                break;
            case "java-settings":
                index = 3;
                break;
            case "language":
                index = 4;
                break;
            case "external-tools":
                index = 5;
                break;
            case "accounts":
                index = 6;
                break;
            case "api":
                index = 7;
                break;
            case "proxy":
                index = 8;
                break;
            }
            settingsWindowLoader.item.currentPageIndex = index;
        });
    }
    function openInstanceSettingsPage(instanceId, pageKey) {
        if (pageKey === "backups") {
            showBackupDialog(instanceId);
            return;
        }
        if (pageKey === "export") {
            showExportDialog(instanceId);
            return;
        }
        instanceSettingsWindowLoader.instanceId = instanceId || "";
        instanceSettingsWindowLoader.active = true;
        Qt.callLater(function () {
            if (!instanceSettingsWindowLoader.item)
                return;
            var index = 0;
            switch (pageKey) {
            case "version":
                index = 0;
                break;
            case "mods":
                index = 1;
                break;
            case "resourcepacks":
                index = 2;
                break;
            case "shaderpacks":
                index = 3;
                break;
            case "texturepacks":
                index = 4;
                break;
            case "worlds":
                index = 5;
                break;
            case "screenshots":
                index = 6;
                break;
            case "servers":
                index = 7;
                break;
            case "gameoptions":
                index = 8;
                break;
            case "settings":
                index = 9;
                break;
            case "notes":
                index = 10;
                break;
            case "console":
                index = 11;
                break;
            }
            instanceSettingsWindowLoader.item.currentPageIndex = index;
        });
    }

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
            color: ThemeColors.background
            flags: Qt.Window
            visible: true

            onClosing: newInstanceWindowLoader.active = false

            property string selectedIconKey: "default"
            property int currentPageIndex: 0
            function openWithParams(initialGroup, importUrl) {
                if (initialGroup && groupCombo) {
                    groupCombo.editText = initialGroup;
                }
                if (importUrl && importPage) {
                    importPage.importUrl = importUrl;
                    newInstanceWindow.currentPageIndex = 1;
                    if (pageList) {
                        pageList.currentIndex = 1;
                    }
                }
            }

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 0
                spacing: 0

                // === Top Section: Icon + Name + Group (like Qt UI) ===
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 100
                    color: ThemeColors.surface

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

                            onClicked: iconPickerPopup.open()

                            ToolTip.text: qsTr("Click to change icon")
                            ToolTip.visible: hovered

                            Popup {
                                id: iconPickerPopup
                                x: iconButton.width + Theme.spacingS
                                y: 0
                                width: 320
                                height: 280
                                modal: true

                                background: Rectangle {
                                    color: ThemeColors.surface
                                    border.color: ThemeColors.border
                                    radius: Theme.radius
                                }

                                ColumnLayout {
                                    anchors.fill: parent
                                    anchors.margins: Theme.spacingS
                                    spacing: Theme.spacingS

                                    Label {
                                        text: qsTr("Select Icon")
                                        color: ThemeColors.text
                                        font.bold: true
                                    }

                                    GridView {
                                        Layout.fillWidth: true
                                        Layout.fillHeight: true
                                        cellWidth: 52
                                        cellHeight: 52
                                        clip: true

                                        model: ["default", "bee", "brick", "chicken", "creeper", "diamond", "dirt", "enderman", "enderpearl", "flame", "fox", "gear", "herobrine", "magitech", "meat", "modrinth", "netherstar", "planks", "skeleton", "squarecreeper", "steve", "stone"]

                                        delegate: Button {
                                            width: 48
                                            height: 48

                                            background: Rectangle {
                                                color: newInstanceWindow.selectedIconKey === modelData ? ThemeColors.accent : (parent.hovered ? "#3d4d60" : "transparent")
                                                radius: Theme.radius
                                                border.color: newInstanceWindow.selectedIconKey === modelData ? ThemeColors.accent : "transparent"
                                                border.width: 2
                                            }

                                            Image {
                                                anchors.centerIn: parent
                                                width: 40
                                                height: 40
                                                source: Theme.instanceIconFromKey(modelData)
                                                fillMode: Image.PreserveAspectFit
                                            }

                                            onClicked: {
                                                newInstanceWindow.selectedIconKey = modelData;
                                                iconPickerPopup.close();
                                            }
                                        }
                                    }
                                }
                            }
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
                                color: ThemeColors.text
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
                                color: ThemeColors.text
                            }

                            ComboBox {
                                id: groupCombo
                                Layout.fillWidth: true
                                editable: true
                                model: {
                                    var groups = ProjT.instancesVM ? ProjT.instancesVM.groupList : null;
                                    if (groups && groups.length > 0) {
                                        return groups;
                                    }
                                    return [""];
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
                    color: ThemeColors.border
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
                        color: ThemeColors.surface

                        ListView {
                            id: pageList
                            anchors.fill: parent
                            anchors.margins: Theme.spacingS
                            spacing: 2
                            currentIndex: newInstanceWindow.currentPageIndex

                            model: ListModel {
                                ListElement {
                                    name: "Custom"
                                    icon: "grass"
                                    platform: ""
                                }
                                ListElement {
                                    name: "Import"
                                    icon: "viewfolder"
                                    platform: ""
                                }
                                ListElement {
                                    name: "ATLauncher"
                                    icon: "gear"
                                    platform: "atlauncher"
                                }
                                ListElement {
                                    name: "CurseForge"
                                    icon: "flame"
                                    platform: "curseforge"
                                }
                                ListElement {
                                    name: "FTB Legacy"
                                    icon: "ftb_logo"
                                    platform: "ftb"
                                }
                                ListElement {
                                    name: "FTB App"
                                    icon: "ftb_logo"
                                    platform: "ftb"
                                }
                                ListElement {
                                    name: "Modrinth"
                                    icon: "modrinth"
                                    platform: "modrinth"
                                }
                                ListElement {
                                    name: "Technic"
                                    icon: "brick"
                                    platform: "technic"
                                }
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
                                        source: model.platform ? Theme.platformIcon(model.platform) : (model.icon === "viewfolder" ? Theme.icon("viewfolder") : "qrc:/icons/multimc/scalable/instances/" + model.icon + ".svg")
                                        fillMode: Image.PreserveAspectFit
                                    }

                                    Label {
                                        text: model.name
                                        color: ThemeColors.text
                                        Layout.fillWidth: true
                                    }
                                }

                                onClicked: {
                                    pageList.currentIndex = index;
                                    newInstanceWindow.currentPageIndex = index;
                                }
                            }
                        }
                    }

                    // Separator
                    Rectangle {
                        Layout.preferredWidth: 1
                        Layout.fillHeight: true
                        color: ThemeColors.border
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

                        // ATLauncher page
                        Loader {
                            source: "modplatform/ATLauncherPage.qml"
                        }

                        // CurseForge page
                        Loader {
                            source: "modplatform/CurseForgePage.qml"
                        }

                        // FTB Legacy placeholder (use FTBPage)
                        Loader {
                            source: "modplatform/FTBPage.qml"
                        }

                        // FTB App (use same FTBPage)
                        Loader {
                            source: "modplatform/FTBPage.qml"
                        }

                        // Modrinth page
                        Loader {
                            source: "modplatform/ModrinthPage.qml"
                        }

                        // Technic page
                        Loader {
                            source: "modplatform/TechnicPage.qml"
                        }
                    }
                }

                // Separator
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 1
                    color: ThemeColors.border
                }

                // === Bottom buttons ===
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 50
                    color: ThemeColors.surface

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: Theme.spacingM
                        spacing: Theme.spacingM

                        ThemedButton {
                            text: qsTr("Help")
                            flat: true
                            onClicked: {
                                // Open help URL
                                Qt.openUrlExternally("https://projtlauncher.yongdohyun.org.tr/wiki/getting-started/create-instance/");
                            }
                        }

                        Item {
                            Layout.fillWidth: true
                        }

                        ThemedButton {
                            text: qsTr("OK")
                            primary: true
                            enabled: instNameField.text.length > 0
                            onClicked: {
                                if (ProjT.instancesVM && instNameField.text.length > 0) {
                                    var version = customPage.selectedVersion || "";
                                    var group = groupCombo.currentText || "";
                                    ProjT.instancesVM.createNewInstance(instNameField.text, version, group);
                                    // Refresh instance list after creation
                                    Qt.callLater(function () {
                                        if (ProjT.instancesVM) {
                                            ProjT.instancesVM.refreshInstances();
                                        }
                                    });
                                }
                                newInstanceWindow.close();
                            }
                        }

                        ThemedButton {
                            text: qsTr("Cancel")
                            outline: true
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
                color: ThemeColors.text
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
                ProjT.instancesVM.renameSelectedInstance(renameField.text);
            }
        }

        onOpened: {
            currentName = ProjT.instancesVM ? ProjT.instancesVM.selectedInstanceName : "";
            renameField.text = currentName;
            renameField.selectAll();
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
                color: ThemeColors.text
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
                ProjT.instancesVM.duplicateSelectedInstance(duplicateNameField.text);
            }
        }

        onOpened: {
            currentName = ProjT.instancesVM ? ProjT.instancesVM.selectedInstanceName : "";
            duplicateNameField.text = currentName + qsTr(" Copy");
            duplicateNameField.selectAll();
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
            currentName = ProjT.instancesVM ? ProjT.instancesVM.selectedInstanceName : "";
        }

        Label {
            text: qsTr("Delete \"%1\"?\n\nThis action cannot be undone.").arg(deleteDialog.currentName)
            color: ThemeColors.error
            wrapMode: Text.WordWrap
            width: parent.width
        }

        onAccepted: {
            if (ProjT.instancesVM) {
                ProjT.instancesVM.deleteSelectedInstance();
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
                color: ThemeColors.text
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
                ProjT.instancesVM.setSelectedGroup(groupNameField.text);
            }
        }

        onOpened: {
            groupNameField.text = "";
        }
    }

    function handlePageRequest(page) {
        switch (page) {
        case LauncherViewModelEnums.Page.Instances:
            break;
        case LauncherViewModelEnums.Page.News:
            showNewsWindow();
            break;
        case LauncherViewModelEnums.Page.Settings:
            showSettingsWindow();
            break;
        case LauncherViewModelEnums.Page.About:
            showAboutWindow();
            break;
        case LauncherViewModelEnums.Page.Logs:
            showLogsWindow();
            break;
        }
    }

    Connections {
        target: ProjT.launcherVM
        function onCurrentPageChanged() {
            if (ProjT.launcherVM) {
                handlePageRequest(ProjT.launcherVM.currentPage);
            }
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
                showSettingsWindow();
            }

            onShowAbout: {
                showAboutWindow();
            }

            onShowLogs: {
                showLogsWindow();
            }

            onCheckUpdate: {
                if (ProjT.launcherVM) {
                    ProjT.launcherVM.checkUpdates();
                }
            }

            onAccountsMenuRequested: {
                console.log("[ShellRoot] Accounts menu requested - opening Settings > Accounts");
                showSettingsOnAccountsPage();
            }

            // Folder actions (using DesktopServices)
            onOpenLauncherFolder: {
                if (ProjT.launcherVM) {
                    ProjT.launcherVM.openLauncherFolder();
                }
            }

            onOpenInstancesFolder: {
                if (ProjT.launcherVM) {
                    ProjT.launcherVM.openInstancesFolder();
                }
            }

            onOpenModsFolder: {
                if (ProjT.launcherVM) {
                    ProjT.launcherVM.openModsFolder();
                }
            }

            onOpenSkinsFolder: {
                if (ProjT.launcherVM) {
                    ProjT.launcherVM.openSkinsFolder();
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

                onCreateNewInstance: {
                    showNewInstanceWindow();
                }
            }

            // === Separator Line ===
            Rectangle {
                visible: isInstancesPage
                Layout.preferredWidth: 1
                Layout.fillHeight: true
                color: ThemeColors.border
            }

            // === INSTANCE TOOLBAR (right vertical toolbar) ===
            InstanceToolBar {
                id: instanceToolBar
                visible: isInstancesPage
                Layout.preferredWidth: 110
                Layout.fillHeight: true

                onEditInstance: {
                    console.log("[ShellRoot] Edit instance requested");
                    if (ProjT.instancesVM && ProjT.instancesVM.selectedInstanceId) {
                        showInstanceSettingsWindow(ProjT.instancesVM.selectedInstanceId);
                    }
                }

                onChangeGroup: changeGroupDialog.open()

                onExportInstance: {
                    console.log("[ShellRoot] Export instance requested");
                    if (ProjT.instancesVM && ProjT.instancesVM.selectedInstanceId) {
                        showExportDialog(ProjT.instancesVM.selectedInstanceId);
                    }
                }

                onManageBackups: {
                    console.log("[ShellRoot] Manage backups requested");
                    if (ProjT.instancesVM && ProjT.instancesVM.selectedInstanceId) {
                        showBackupDialog(ProjT.instancesVM.selectedInstanceId);
                    }
                }

                onCopyInstance: duplicateDialog.open()

                onDeleteInstance: deleteDialog.open()

                onCreateShortcut: {
                    console.log("[ShellRoot] Create shortcut requested");
                    createShortcutDialogLoader.active = true;
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
                showNewsWindow();
            }

            onNewsClicked: {
                showNewsWindow();
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
    function showSettingsWindow(pageIndex) {
        settingsWindowLoader.active = true;
        if (pageIndex !== undefined && settingsWindowLoader.item) {
            settingsWindowLoader.item.currentPageIndex = pageIndex;
        }
    }
    function showSettingsOnAccountsPage() {
        settingsWindowLoader.active = true;
        // Accounts page is at index 6
        Qt.callLater(function () {
            if (settingsWindowLoader.item) {
                settingsWindowLoader.item.currentPageIndex = 6;
            }
        });
    }
    function showAboutWindow() {
        aboutWindowLoader.active = true;
    }
    function showLogsWindow() {
        logsWindowLoader.active = true;
    }
    function showNewsWindow() {
        newsWindowLoader.active = true;
    }
    function showAccountsWindow() {
        accountsWindowLoader.active = true;
    }
    function openSetupWizard(pageIds) {
        setupWizardLoader.pageIds = pageIds || [];
        setupWizardLoader.active = true;
        Qt.callLater(function () {
            if (setupWizardLoader.item && setupWizardLoader.item.show) {
                setupWizardLoader.item.show();
            }
        });
    }

    // New dialog helper functions
    function showMSALoginDialog() {
        msaLoginDialogLoader.active = true;
    }
    function showOfflineLoginDialog() {
        offlineLoginDialogLoader.active = true;
    }
    function showProgressDialog(title, message) {
        progressDialogLoader.dialogTitle = title || qsTr("Please wait...");
        progressDialogLoader.dialogMessage = message || "";
        progressDialogLoader.active = true;
    }
    function hideProgressDialog() {
        progressDialogLoader.active = false;
    }
    function showUpdateDialog(currentVersion, newVersion, releaseNotes) {
        updateDialogLoader.currentVersion = currentVersion || "";
        updateDialogLoader.newVersion = newVersion || "";
        updateDialogLoader.releaseNotes = releaseNotes || "";
        updateDialogLoader.active = true;
    }
    function showExportDialog(instanceId) {
        exportDialogLoader.instanceId = instanceId || "";
        exportDialogLoader.active = true;
    }
    function showBackupDialog(instanceId) {
        backupDialogLoader.instanceId = instanceId || "";
        backupDialogLoader.active = true;
    }
    function showIconPickerDialog() {
        iconPickerDialogLoader.active = true;
    }
    function showBlockedModsDialog(mods) {
        blockedModsDialogLoader.blockedMods = mods || [];
        blockedModsDialogLoader.active = true;
    }
    function showInstanceSettingsWindow(instanceId) {
        instanceSettingsWindowLoader.instanceId = instanceId || "";
        instanceSettingsWindowLoader.active = true;
    }

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
            color: ThemeColors.background
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
                        color: ThemeColors.surface

                        ListView {
                            id: settingsPageList
                            anchors.fill: parent
                            anchors.margins: Theme.spacingS
                            spacing: 2
                            currentIndex: settingsWindow.currentPageIndex

                            model: ListModel {
                                ListElement {
                                    name: "Launcher"
                                    iconName: "settings"
                                }
                                ListElement {
                                    name: "Appearance"
                                    iconName: "appearance"
                                }
                                ListElement {
                                    name: "Minecraft"
                                    iconName: "minecraft"
                                }
                                ListElement {
                                    name: "Java"
                                    iconName: "java"
                                }
                                ListElement {
                                    name: "Language"
                                    iconName: "language"
                                }
                                ListElement {
                                    name: "External Tools"
                                    iconName: "externaltools"
                                }
                                ListElement {
                                    name: "Accounts"
                                    iconName: "accounts"
                                }
                                ListElement {
                                    name: "API"
                                    iconName: "news"
                                }
                                ListElement {
                                    name: "Proxy"
                                    iconName: "proxy"
                                }
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
                                    settingsPageList.currentIndex = index;
                                    settingsWindow.currentPageIndex = index;
                                }
                            }
                        }
                    }

                    // Separator
                    Rectangle {
                        Layout.preferredWidth: 1
                        Layout.fillHeight: true
                        color: ThemeColors.border
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
                            color: ThemeColors.surface

                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: Theme.spacingM
                                anchors.rightMargin: Theme.spacingM

                                Label {
                                    text: settingsPageList.model.get(settingsWindow.currentPageIndex).name
                                    font.pointSize: 12
                                    font.bold: true
                                    color: ThemeColors.text
                                }

                                Item {
                                    Layout.fillWidth: true
                                }
                            }
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 1
                            color: ThemeColors.border
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
                    color: ThemeColors.border
                }

                // === Bottom buttons ===
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 50
                    color: ThemeColors.surface

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: Theme.spacingM
                        spacing: Theme.spacingM

                        ThemedButton {
                            text: qsTr("Help")
                            flatStyle: true
                            onClicked: {
                                Qt.openUrlExternally("https://projtlauncher.yongdohyun.org.tr/wiki/");
                            }
                        }

                        Item {
                            Layout.fillWidth: true
                        }

                        ThemedButton {
                            text: qsTr("OK")
                            primary: true
                            onClicked: {
                                // Apply settings and close
                                if (ProjT.settingsVM) {
                                    ProjT.settingsVM.applyChanges();
                                }
                                if (App && App.notifyGlobalSettingsApplied) {
                                    App.notifyGlobalSettingsApplied();
                                }
                                settingsWindow.close();
                            }
                        }

                        ThemedButton {
                            text: qsTr("Cancel")
                            outline: true
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
            color: ThemeColors.background
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
            color: ThemeColors.background
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
            color: ThemeColors.background
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
            color: ThemeColors.background
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
                    color: ThemeColors.text
                }

                Label {
                    text: qsTr("Manage your Minecraft accounts below.")
                    color: ThemeColors.textSecondary
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                // Account list placeholder
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: ThemeColors.surface
                    radius: Theme.radiusS
                    border.color: ThemeColors.border
                    border.width: 1

                    Label {
                        anchors.centerIn: parent
                        text: qsTr("No accounts added yet")
                        color: ThemeColors.textSecondary
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingM

                    ThemedButton {
                        text: qsTr("Add Microsoft Account")
                        primary: true
                        onClicked: {
                            showMSALoginDialog();
                        }
                    }

                    ThemedButton {
                        text: qsTr("Add Offline Account")
                        success: true
                        onClicked: {
                            showOfflineLoginDialog();
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    ThemedButton {
                        text: qsTr("Close")
                        outline: true
                        onClicked: accountsWindow.close()
                    }
                }
            }
        }
    }

    // === Setup Wizard (first-run) ===
    Loader {
        id: setupWizardLoader
        active: false
        property var pageIds: []
        sourceComponent: SetupWizard {
            id: setupWizardDialog
            pageIds: setupWizardLoader.pageIds
            visible: true
            onAccepted: setupWizardLoader.active = false
            onRejected: setupWizardLoader.active = false
            onClosed: setupWizardLoader.active = false
        }
    }

    // ════════════════════════════════════════════════════════════════
    // NEW DIALOG LOADERS
    // ════════════════════════════════════════════════════════════════

    // MSA Login Dialog
    Loader {
        id: msaLoginDialogLoader
        active: false
        sourceComponent: MSALoginDialog {
            id: msaLoginDialog
            visible: true
            onClosed: msaLoginDialogLoader.active = false
        }
    }

    // Offline Login Dialog
    Loader {
        id: offlineLoginDialogLoader
        active: false
        sourceComponent: OfflineLoginDialog {
            id: offlineLoginDialog
            visible: true
            onClosed: offlineLoginDialogLoader.active = false
        }
    }

    // Progress Dialog
    Loader {
        id: progressDialogLoader
        active: false
        property string dialogTitle: ""
        property string dialogMessage: ""
        sourceComponent: ProgressDialog {
            id: progressDialog
            title: progressDialogLoader.dialogTitle
            globalStatus: progressDialogLoader.dialogMessage
            visible: true
            onClosed: progressDialogLoader.active = false
        }
    }

    // Update Dialog
    Loader {
        id: updateDialogLoader
        active: false
        property string currentVersion: ""
        property string newVersion: ""
        property string releaseNotes: ""
        sourceComponent: UpdateDialog {
            id: updateDialog
            currentVersion: updateDialogLoader.currentVersion
            newVersion: updateDialogLoader.newVersion
            releaseNotes: updateDialogLoader.releaseNotes
            visible: true
            onClosed: updateDialogLoader.active = false
        }
    }

    // Export Dialog
    Loader {
        id: exportDialogLoader
        active: false
        property string instanceId: ""
        sourceComponent: ExportDialog {
            id: exportDialog
            visible: true
            onClosed: exportDialogLoader.active = false
        }
    }

    // Backup Dialog
    Loader {
        id: backupDialogLoader
        active: false
        property string instanceId: ""
        sourceComponent: BackupDialog {
            id: backupDialog
            visible: true
            onClosed: backupDialogLoader.active = false
        }
    }

    // Icon Picker Dialog
    Loader {
        id: iconPickerDialogLoader
        active: false
        sourceComponent: IconPickerDialog {
            id: iconPickerDialog
            visible: true
            onClosed: iconPickerDialogLoader.active = false
            onIconSelected: function (iconKey) {
                console.log("[ShellRoot] Icon selected:", iconKey);
            }
        }
    }

    // Blocked Mods Dialog
    Loader {
        id: blockedModsDialogLoader
        active: false
        property var blockedMods: []
        sourceComponent: BlockedModsDialog {
            id: blockedModsDialog
            blockedMods: blockedModsDialogLoader.blockedMods
            visible: true
            onClosed: blockedModsDialogLoader.active = false
        }
    }

    // Create Shortcut Dialog
    Loader {
        id: createShortcutDialogLoader
        active: false
        sourceComponent: CreateShortcutDialog {
            id: createShortcutDialog
            instance: ProjT.instancesVM ? {
                id: ProjT.instancesVM.selectedInstanceId,
                name: ProjT.instancesVM.selectedInstanceName,
                iconPath: ProjT.instancesVM.selectedInstanceIcon,
                version: ProjT.instancesVM.selectedInstanceVersion
            } : null
            visible: true
            onClosed: createShortcutDialogLoader.active = false
        }
    }

    // Copy Instance Dialog
    Loader {
        id: copyInstanceDialogLoader
        active: false
        sourceComponent: CopyInstanceDialog {
            id: copyInstanceDialog
            sourceInstance: ProjT.instancesVM ? {
                id: ProjT.instancesVM.selectedInstanceId,
                name: ProjT.instancesVM.selectedInstanceName,
                iconPath: ProjT.instancesVM.selectedInstanceIcon,
                version: ProjT.instancesVM.selectedInstanceVersion
            } : null
            visible: true
            onClosed: copyInstanceDialogLoader.active = false
        }
    }

    // Version Select Dialog
    Loader {
        id: versionSelectDialogLoader
        active: false
        sourceComponent: VersionSelectDialog {
            id: versionSelectDialog
            vm: ProjT.instanceVM
            visible: true
            onClosed: versionSelectDialogLoader.active = false
        }
    }

    // Install Loader Dialog
    Loader {
        id: installLoaderDialogLoader
        active: false
        property string minecraftVersion: ""
        sourceComponent: InstallLoaderDialog {
            id: installLoaderDialog
            vm: ProjT.instanceVM
            minecraftVersion: installLoaderDialogLoader.minecraftVersion
            visible: true
            onClosed: installLoaderDialogLoader.active = false
        }
    }

    // Resource Download Dialog
    Loader {
        id: resourceDownloadDialogLoader
        active: false
        property string resourceType: "mod"
        sourceComponent: ResourceDownloadDialog {
            id: resourceDownloadDialog
            vm: ProjT.instanceVM
            resourceType: resourceDownloadDialogLoader.resourceType
            visible: true
            onClosed: resourceDownloadDialogLoader.active = false
        }
    }

    // Resource Update Dialog
    Loader {
        id: resourceUpdateDialogLoader
        active: false
        sourceComponent: ResourceUpdateDialog {
            id: resourceUpdateDialog
            vm: ProjT.instanceVM
            visible: true
            onClosed: resourceUpdateDialogLoader.active = false
        }
    }

    // ════════════════════════════════════════════════════════════════
    // INSTANCE SETTINGS WINDOW (Per-instance settings with tabs)
    // ════════════════════════════════════════════════════════════════
    Loader {
        id: instanceSettingsWindowLoader
        active: false
        property string instanceId: ""
        onInstanceIdChanged: {
            if (instanceSettingsWindowLoader.item && instanceSettingsWindowLoader.item.syncSettingsVM) {
                instanceSettingsWindowLoader.item.syncSettingsVM();
            }
        }
        sourceComponent: Window {
            id: instanceSettingsWindow
            title: qsTr("Instance Settings")
            width: 900
            height: 650
            minimumWidth: 700
            minimumHeight: 500
            color: ThemeColors.background
            flags: Qt.Window
            visible: true

            onClosing: instanceSettingsWindowLoader.active = false

            property int currentPageIndex: 0
            property var instanceVM: ProjT.instanceVM

            function syncSettingsVM() {
                if (!ProjT.settingsVM || !instanceSettingsWindowLoader.instanceId)
                    return;
                if (ProjT.settingsVM.instanceId !== instanceSettingsWindowLoader.instanceId) {
                    ProjT.settingsVM.instanceId = instanceSettingsWindowLoader.instanceId;
                }
                ProjT.settingsVM.refresh();
            }

            Component.onCompleted: {
                if (instanceVM && instanceSettingsWindowLoader.instanceId) {
                    instanceVM.instanceId = instanceSettingsWindowLoader.instanceId;
                }
                syncSettingsVM();
            }

            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                // Page Container Area
                RowLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    spacing: 0

                    // Left sidebar - Category list
                    Rectangle {
                        Layout.preferredWidth: 180
                        Layout.fillHeight: true
                        color: ThemeColors.surface

                        ListView {
                            id: instancePageList
                            anchors.fill: parent
                            anchors.margins: Theme.spacingS
                            spacing: 2
                            currentIndex: instanceSettingsWindow.currentPageIndex

                            model: ListModel {
                                ListElement {
                                    name: "Version"
                                    iconName: "minecraft"
                                }
                                ListElement {
                                    name: "Mods"
                                    iconName: "loadermods"
                                }
                                ListElement {
                                    name: "Resource Packs"
                                    iconName: "resourcepacks"
                                }
                                ListElement {
                                    name: "Shader Packs"
                                    iconName: "shaderpacks"
                                }
                                ListElement {
                                    name: "Texture Packs"
                                    iconName: "resourcepacks"
                                }
                                ListElement {
                                    name: "Worlds"
                                    iconName: "worlds"
                                }
                                ListElement {
                                    name: "Screenshots"
                                    iconName: "screenshots"
                                }
                                ListElement {
                                    name: "Servers"
                                    iconName: "servers"
                                }
                                ListElement {
                                    name: "Game Options"
                                    iconName: "settings"
                                }
                                ListElement {
                                    name: "Settings"
                                    iconName: "settings"
                                }
                                ListElement {
                                    name: "Notes"
                                    iconName: "notes"
                                }
                                ListElement {
                                    name: "Log"
                                    iconName: "log"
                                }
                            }

                            delegate: ItemDelegate {
                                width: instancePageList.width
                                height: 40
                                highlighted: instancePageList.currentIndex === index

                                text: model.name

                                onClicked: {
                                    instancePageList.currentIndex = index;
                                    instanceSettingsWindow.currentPageIndex = index;
                                }
                            }
                        }
                    }

                    // Separator
                    Rectangle {
                        Layout.preferredWidth: 1
                        Layout.fillHeight: true
                        color: ThemeColors.border
                    }

                    // Right content area
                    StackLayout {
                        id: instancePageStack
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        currentIndex: instanceSettingsWindow.currentPageIndex

                        // Version page
                        VersionPage {}

                        // Mods page
                        ModsPage {}

                        // Resource Packs page
                        ResourcePacksPage {}

                        // Shader Packs page
                        ShaderPacksPage {}

                        // Texture Packs page
                        TexturePacksPage {}

                        // Worlds page (from settings folder)
                        Loader {
                            source: "qrc:/qml/settings/WorldsPage.qml"
                        }

                        // Screenshots page (from settings folder)
                        Loader {
                            source: "qrc:/qml/settings/ScreenshotsPage.qml"
                        }

                        // Servers page (from settings folder)
                        Loader {
                            source: "qrc:/qml/settings/ServersPage.qml"
                        }

                        // Game Options page
                        GameOptionsPage {}

                        // Instance Settings page
                        SettingsPage {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                        }

                        // Notes page (from settings folder)
                        Loader {
                            source: "qrc:/qml/settings/NotesPage.qml"
                        }

                        // Log page
                        LogPage {}
                    }
                }

                // Bottom buttons
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 50
                    color: ThemeColors.surface

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: Theme.spacingM
                        spacing: Theme.spacingM

                        Button {
                            text: qsTr("Launch")
                            highlighted: true
                            icon.name: "media-playback-start"
                            onClicked: {
                                if (ProjT.instancesVM) {
                                    ProjT.instancesVM.launchSelectedInstance();
                                }
                                instanceSettingsWindow.close();
                            }
                        }

                        Item {
                            Layout.fillWidth: true
                        }

                        Button {
                            text: qsTr("Close")
                            onClicked: instanceSettingsWindow.close()
                        }
                    }
                }
            }
        }
    }

    // ════════════════════════════════════════════════════════════════
    // MODPLATFORM BROWSER WINDOW
    // ════════════════════════════════════════════════════════════════
    Loader {
        id: modplatformWindowLoader
        active: false
        property string platform: "curseforge"
        sourceComponent: Window {
            id: modplatformWindow
            title: {
                switch (modplatformWindowLoader.platform) {
                case "curseforge":
                    return "CurseForge";
                case "modrinth":
                    return "Modrinth";
                case "atlauncher":
                    return "ATLauncher";
                case "ftb":
                    return "FTB";
                case "technic":
                    return "Technic";
                default:
                    return qsTr("Browse Modpacks");
                }
            }
            width: 900
            height: 650
            minimumWidth: 700
            minimumHeight: 500
            color: ThemeColors.background
            flags: Qt.Window
            visible: true

            onClosing: modplatformWindowLoader.active = false

            Loader {
                anchors.fill: parent
                source: {
                    switch (modplatformWindowLoader.platform) {
                    case "curseforge":
                        return "modplatform/CurseForgePage.qml";
                    case "modrinth":
                        return "modplatform/ModrinthPage.qml";
                    case "atlauncher":
                        return "modplatform/ATLauncherPage.qml";
                    case "ftb":
                        return "modplatform/FTBPage.qml";
                    case "technic":
                        return "modplatform/TechnicPage.qml";
                    default:
                        return "";
                    }
                }
            }
        }
    }

    // Helper function to open modplatform browser
    function showModplatformBrowser(platform) {
        modplatformWindowLoader.platform = platform || "curseforge";
        modplatformWindowLoader.active = true;
    }
}
