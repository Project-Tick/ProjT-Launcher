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

ScrollView {
    id: launcherPage
    clip: true
    
    property var vm: ProjT.launcherSettingsVM
    
    ColumnLayout {
        width: launcherPage.width - Theme.spacingL
        spacing: Theme.spacingM
        
        // User Interface
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("User Interface")
            
            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS
                
                Label {
                    text: qsTr("Instance Sorting")
                    color: ThemeColors.text
                    font.bold: true
                }
                
                RadioButton {
                    id: sortByNameRadio
                    text: qsTr("By &name")
                    checked: vm ? vm.sortByName : true
                    onCheckedChanged: {
                        if (vm && checked) vm.sortByName = true
                    }
                }
                
                RadioButton {
                    text: qsTr("&By last launched")
                    checked: vm ? !vm.sortByName : false
                    onCheckedChanged: {
                        if (vm && checked) vm.sortByName = false
                    }
                }
                
                Item { height: 6 }
                
                Label {
                    text: qsTr("Instance Renaming")
                    color: ThemeColors.text
                    font.bold: true
                }
                
                RadioButton {
                    text: qsTr("Ask what to do")
                    checked: vm ? vm.renamingBehavior === "ask" : true
                    onCheckedChanged: {
                        if (vm && checked) vm.renamingBehavior = "ask"
                    }
                }
                
                RadioButton {
                    text: qsTr("Always rename the folder")
                    checked: vm ? vm.renamingBehavior === "always" : false
                    onCheckedChanged: {
                        if (vm && checked) vm.renamingBehavior = "always"
                    }
                }
                
                RadioButton {
                    text: qsTr("Never rename the folder")
                    checked: vm ? vm.renamingBehavior === "never" : false
                    onCheckedChanged: {
                        if (vm && checked) vm.renamingBehavior = "never"
                    }
                }
                
                Item { height: 6 }
                
                CheckBox {
                    text: qsTr("&Replace toolbar with menubar")
                    ToolTip.text: qsTr("The menubar is more friendly for keyboard-driven interaction.")
                    ToolTip.visible: hovered
                    checked: vm ? vm.preferMenuBar : false
                    onCheckedChanged: if (vm) vm.preferMenuBar = checked
                }
            }
        }
        
        // Updater Settings
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Updater")
            
            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS
                
                CheckBox {
                    id: autoUpdateCheckBox
                    text: qsTr("Check for updates automatically")
                    checked: vm ? vm.autoUpdateCheck : true
                    onCheckedChanged: if (vm) vm.autoUpdateCheck = checked
                }
                
                RowLayout {
                    spacing: Theme.spacingS
                    
                    Label {
                        text: qsTr("How Often?")
                        color: ThemeColors.text
                    }
                    
                    SpinBox {
                        id: updateIntervalSpinBox
                        from: 0
                        to: 168
                        value: vm ? vm.updateInterval : 0
                        textFromValue: function(value) {
                            if (value === 0) return qsTr("On Launch")
                            return qsTr("Every %1 hours").arg(value)
                        }
                        ToolTip.text: qsTr("Set to 0 to only check on launch")
                        ToolTip.visible: hovered
                        onValueModified: if (vm) vm.updateInterval = value
                    }
                    
                    Item { Layout.fillWidth: true }
                }
            }
        }
        
        // Folders
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Folders")
            
            GridLayout {
                anchors.fill: parent
                columns: 3
                rowSpacing: Theme.spacingS
                columnSpacing: Theme.spacingS
                
                Label {
                    text: qsTr("I&nstances:")
                    color: ThemeColors.text
                }
                TextField {
                    id: instDirTextBox
                    Layout.fillWidth: true
                    text: vm ? vm.instancesFolder : ""
                    readOnly: true
                }
                Button {
                    text: qsTr("Browse")
                    onClicked: browseForInstancesFolder()
                }
                
                Label {
                    text: qsTr("&Mods:")
                    color: ThemeColors.text
                }
                TextField {
                    id: modsDirTextBox
                    Layout.fillWidth: true
                    text: vm ? vm.modsFolder : ""
                    readOnly: true
                }
                Button {
                    text: qsTr("Browse")
                    onClicked: browseForModsFolder()
                }
                
                Label {
                    text: qsTr("&Icons:")
                    color: ThemeColors.text
                }
                TextField {
                    id: iconsDirTextBox
                    Layout.fillWidth: true
                    text: vm ? vm.iconsFolder : ""
                    readOnly: true
                }
                Button {
                    text: qsTr("Browse")
                    onClicked: browseForIconsFolder()
                }
                
                Label {
                    text: qsTr("&Java:")
                    color: ThemeColors.text
                }
                TextField {
                    id: javaDirTextBox
                    Layout.fillWidth: true
                    text: vm ? vm.javaFolder : ""
                    readOnly: true
                }
                Button {
                    text: qsTr("Browse")
                    onClicked: browseForJavaFolder()
                }
                
                Label {
                    text: qsTr("&Skins:")
                    color: ThemeColors.text
                }
                TextField {
                    id: skinsDirTextBox
                    Layout.fillWidth: true
                    text: vm ? vm.skinsFolder : ""
                    readOnly: true
                }
                Button {
                    text: qsTr("Browse")
                    onClicked: browseForSkinsFolder()
                }
                
                Label {
                    text: qsTr("&Downloads:")
                    color: ThemeColors.text
                }
                TextField {
                    id: downloadsDirTextBox
                    Layout.fillWidth: true
                    text: vm ? vm.downloadsFolder : ""
                    readOnly: true
                }
                Button {
                    text: qsTr("Browse")
                    onClicked: browseForDownloadsFolder()
                }
            }
        }
        
        // Mods and Modpacks
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Mods and Modpacks")
            
            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS
                
                CheckBox {
                    text: qsTr("Check &subfolders for blocked mods")
                    ToolTip.text: qsTr("When enabled, in addition to the downloads folder, its sub folders will also be searched when looking for resources (e.g. when looking for blocked mods on CurseForge).")
                    ToolTip.visible: hovered
                    checked: vm ? vm.downloadsDirWatchRecursive : false
                    onCheckedChanged: if (vm) vm.downloadsDirWatchRecursive = checked
                }
                
                CheckBox {
                    text: qsTr("Move blocked mods instead of copying them")
                    ToolTip.text: qsTr("When enabled, it will move blocked resources instead of copying them.")
                    ToolTip.visible: hovered
                    checked: vm ? vm.downloadsDirMove : false
                    onCheckedChanged: if (vm) vm.downloadsDirMove = checked
                }
                
                CheckBox {
                    id: metadataEnableBtn
                    text: qsTr("Keep track of mod metadata")
                    ToolTip.text: qsTr("Store version information provided by mod providers (like Modrinth or CurseForge) for mods.")
                    ToolTip.visible: hovered
                    checked: vm ? vm.metadataEnabled : true
                    onCheckedChanged: if (vm) vm.metadataEnabled = checked
                }
                
                Label {
                    visible: !metadataEnableBtn.checked
                    text: qsTr("<span style='font-weight:600; color:#f5c211;'>Warning</span><span style='color:#f5c211;'>: Disabling mod metadata may also disable some QoL features, such as mod updating!</span>")
                    textFormat: Text.RichText
                    wrapMode: Text.Wrap
                    Layout.fillWidth: true
                }
                
                CheckBox {
                    text: qsTr("Install dependencies automatically")
                    ToolTip.text: qsTr("Automatically detect, install, and update mod dependencies.")
                    ToolTip.visible: hovered
                    checked: vm ? vm.dependenciesEnabled : true
                    onCheckedChanged: if (vm) vm.dependenciesEnabled = checked
                }
                
                CheckBox {
                    text: qsTr("Suggest to update an existing instance during modpack installation")
                    ToolTip.text: qsTr("When creating a new modpack instance, suggest updating an existing instance instead.")
                    ToolTip.visible: hovered
                    checked: vm ? vm.modpackUpdatePrompt : true
                    onCheckedChanged: if (vm) vm.modpackUpdatePrompt = checked
                }
            }
        }
        
        // Console
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Console")
            
            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS
                
                RowLayout {
                    spacing: Theme.spacingS
                    
                    Label {
                        text: qsTr("Log History &Limit:")
                        color: ThemeColors.text
                    }
                    
                    SpinBox {
                        id: lineLimitSpinBox
                        from: 10000
                        to: 1000000
                        stepSize: 10000
                        value: vm ? vm.logHistoryLimit : 100000
                        textFromValue: function(value) {
                            return value + qsTr(" lines")
                        }
                        onValueModified: if (vm) vm.logHistoryLimit = value
                    }
                }
                
                CheckBox {
                    text: qsTr("&Stop logging when log overflows")
                    checked: vm ? vm.stopLoggingOnOverflow : false
                    onCheckedChanged: if (vm) vm.stopLoggingOnOverflow = checked
                }
            }
        }
        
        // Tasks
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Tasks")
            
            GridLayout {
                anchors.fill: parent
                columns: 3
                rowSpacing: Theme.spacingS
                columnSpacing: Theme.spacingS
                
                Label {
                    text: qsTr("Concurrent Task Limit:")
                    color: ThemeColors.text
                }
                SpinBox {
                    id: numberOfConcurrentTasksSpinBox
                    from: 1
                    to: 20
                    value: vm ? vm.concurrentTasks : 4
                    implicitWidth: 80
                    onValueModified: if (vm) vm.concurrentTasks = value
                }
                Item { Layout.fillWidth: true }
                
                Label {
                    text: qsTr("Concurrent Download Limit:")
                    color: ThemeColors.text
                }
                SpinBox {
                    id: numberOfConcurrentDownloadsSpinBox
                    from: 1
                    to: 20
                    value: vm ? vm.concurrentDownloads : 6
                    implicitWidth: 80
                    onValueModified: if (vm) vm.concurrentDownloads = value
                }
                Item { Layout.fillWidth: true }
                
                Label {
                    text: qsTr("Retry Limit:")
                    color: ThemeColors.text
                }
                SpinBox {
                    id: numberOfManualRetriesSpinBox
                    from: 0
                    to: 10
                    value: vm ? vm.retryLimit : 3
                    implicitWidth: 80
                    onValueModified: if (vm) vm.retryLimit = value
                }
                Item { Layout.fillWidth: true }
                
                Label {
                    text: qsTr("HTTP Timeout:")
                    color: ThemeColors.text
                    ToolTip.text: qsTr("Seconds to wait until the requests are terminated")
                    ToolTip.visible: mouseArea.containsMouse
                    
                    MouseArea {
                        id: mouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                    }
                }
                SpinBox {
                    id: timeoutSecondsSpinBox
                    from: 5
                    to: 300
                    value: vm ? vm.httpTimeout : 30
                    implicitWidth: 80
                    textFromValue: function(value) {
                        return value + "s"
                    }
                    onValueModified: if (vm) vm.httpTimeout = value
                }
                Item { Layout.fillWidth: true }
            }
        }
        
        // Backups
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Backups")
            
            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS
                
                CheckBox {
                    text: qsTr("Automatically backup before launch")
                    ToolTip.text: qsTr("Automatically create a backup of your instance before launching. Backups include saves, config, and options, but not mods.")
                    ToolTip.visible: hovered
                    checked: vm ? vm.autoBackupBeforeLaunch : false
                    onCheckedChanged: if (vm) vm.autoBackupBeforeLaunch = checked
                }
            }
        }
        
        Item { height: Theme.spacingL }
    }
    
    // Browse functions
    function browseForInstancesFolder() {
        if (ProjT.launcherVM && ProjT.launcherVM.browseForFolder) {
            var result = ProjT.launcherVM.browseForFolder(qsTr("Select Instances Folder"))
            if (result && result.length > 0) {
                if (vm) vm.instancesFolder = result
            }
        }
    }
    
    function browseForModsFolder() {
        if (ProjT.launcherVM && ProjT.launcherVM.browseForFolder) {
            var result = ProjT.launcherVM.browseForFolder(qsTr("Select Mods Folder"))
            if (result && result.length > 0) {
                if (vm) vm.modsFolder = result
            }
        }
    }
    
    function browseForIconsFolder() {
        if (ProjT.launcherVM && ProjT.launcherVM.browseForFolder) {
            var result = ProjT.launcherVM.browseForFolder(qsTr("Select Icons Folder"))
            if (result && result.length > 0) {
                if (vm) vm.iconsFolder = result
            }
        }
    }
    
    function browseForJavaFolder() {
        if (ProjT.launcherVM && ProjT.launcherVM.browseForFolder) {
            var result = ProjT.launcherVM.browseForFolder(qsTr("Select Java Folder"))
            if (result && result.length > 0) {
                if (vm) vm.javaFolder = result
            }
        }
    }
    
    function browseForSkinsFolder() {
        if (ProjT.launcherVM && ProjT.launcherVM.browseForFolder) {
            var result = ProjT.launcherVM.browseForFolder(qsTr("Select Skins Folder"))
            if (result && result.length > 0) {
                if (vm) vm.skinsFolder = result
            }
        }
    }
    
    function browseForDownloadsFolder() {
        if (ProjT.launcherVM && ProjT.launcherVM.browseForFolder) {
            var result = ProjT.launcherVM.browseForFolder(qsTr("Select Downloads Folder"))
            if (result && result.length > 0) {
                if (vm) vm.downloadsFolder = result
            }
        }
    }
}
