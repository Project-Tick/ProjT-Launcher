// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  This file is part of ProjT Launcher and is licensed under
 *  the GNU General Public License version 3 or later.
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs
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
                    color: Theme.textPrimary
                    font.bold: true
                }
                
                RadioButton {
                    id: sortByNameRadio
                    text: qsTr("By name")
                    checked: vm ? vm.sortByName : true
                    onCheckedChanged: {
                        if (vm && checked) vm.sortByName = true
                    }
                }
                
                RadioButton {
                    text: qsTr("By last launched")
                    checked: vm ? !vm.sortByName : false
                    onCheckedChanged: {
                        if (vm && checked) vm.sortByName = false
                    }
                }
                
                Item { height: Theme.spacingS }
                
                Label {
                    text: qsTr("Instance Renaming")
                    color: Theme.textPrimary
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
            }
        }
        
        // Console
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Console")
            
            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS
                
                CheckBox {
                    text: qsTr("Show console while the game is running")
                    checked: vm ? vm.showConsole : false
                    onCheckedChanged: if (vm) vm.showConsole = checked
                }
                
                CheckBox {
                    text: qsTr("Automatically close console when the game quits")
                    checked: vm ? vm.autoCloseConsole : true
                    onCheckedChanged: if (vm) vm.autoCloseConsole = checked
                }
                
                CheckBox {
                    text: qsTr("Show console when the game crashes")
                    checked: vm ? vm.showConsoleOnCrash : true
                    onCheckedChanged: if (vm) vm.showConsoleOnCrash = checked
                }
            }
        }
        
        // Actions
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Actions")
            
            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS
                
                Label {
                    text: qsTr("On instance launch")
                    color: Theme.textPrimary
                    font.bold: true
                }
                
                RadioButton {
                    text: qsTr("Do nothing")
                    checked: vm ? vm.launchAction === "doNothing" : true
                    onCheckedChanged: if (vm && checked) vm.launchAction = "doNothing"
                }
                
                RadioButton {
                    text: qsTr("Hide the launcher")
                    checked: vm ? vm.launchAction === "hideWindow" : false
                    onCheckedChanged: if (vm && checked) vm.launchAction = "hideWindow"
                }
                
                RadioButton {
                    text: qsTr("Close the launcher")
                    checked: vm ? vm.launchAction === "closeWindow" : false
                    onCheckedChanged: if (vm && checked) vm.launchAction = "closeWindow"
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
                    text: qsTr("Instances:")
                    color: Theme.textPrimary
                }
                
                TextField {
                    id: instancesFolderField
                    Layout.fillWidth: true
                    text: vm ? vm.instancesFolder : "instances"
                    readOnly: true
                }
                
                Button {
                    text: qsTr("Browse...")
                    onClicked: instancesFolderDialog.open()
                }
                
                Label {
                    text: qsTr("Mods:")
                    color: Theme.textPrimary
                }
                
                TextField {
                    id: modsFolderField
                    Layout.fillWidth: true
                    text: vm ? vm.modsFolder : "mods"
                    readOnly: true
                }
                
                Button {
                    text: qsTr("Browse...")
                    onClicked: modsFolderDialog.open()
                }
                
                Label {
                    text: qsTr("Icons:")
                    color: Theme.textPrimary
                }
                
                TextField {
                    id: iconsFolderField
                    Layout.fillWidth: true
                    text: vm ? vm.iconsFolder : "icons"
                    readOnly: true
                }
                
                Button {
                    text: qsTr("Browse...")
                    onClicked: iconsFolderDialog.open()
                }
            }
        }
        
        // Downloads
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Downloads")
            
            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS
                
                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingM
                    
                    Label {
                        text: qsTr("Concurrent downloads:")
                        color: Theme.textPrimary
                    }
                    
                    SpinBox {
                        from: 1
                        to: 20
                        value: vm ? vm.concurrentDownloads : 6
                        onValueModified: if (vm) vm.concurrentDownloads = value
                    }
                }
                
                CheckBox {
                    text: qsTr("Validate downloads")
                    checked: vm ? vm.validateDownloads : true
                    onCheckedChanged: if (vm) vm.validateDownloads = checked
                }
            }
        }
        
        Item { height: Theme.spacingL }
    }
    
    // Folder dialogs
    FolderDialog {
        id: instancesFolderDialog
        title: qsTr("Select Instances Folder")
        onAccepted: if (vm) vm.instancesFolder = selectedFolder.toString().replace("file://", "")
    }
    
    FolderDialog {
        id: modsFolderDialog
        title: qsTr("Select Mods Folder")
        onAccepted: if (vm) vm.modsFolder = selectedFolder.toString().replace("file://", "")
    }
    
    FolderDialog {
        id: iconsFolderDialog
        title: qsTr("Select Icons Folder")
        onAccepted: if (vm) vm.iconsFolder = selectedFolder.toString().replace("file://", "")
    }
}
