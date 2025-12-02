// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  InstanceToolBar - Vertical toolbar for instance actions (right side)
 *  
 *  Matches Widget MainWindow instanceToolBar layout:
 *  [Launch/Kill] | [Edit] [Group] [Folder] [Export] [Backup] [Copy] [Delete] [Shortcut]
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme.js" as Theme

Rectangle {
    id: instanceToolbar
    color: Theme.surface
    width: 120
    
    // ViewModel reference
    readonly property var vm: ProjT.instancesVM
    
    // Instance state
    readonly property bool hasSelection: vm && vm.selectedInstanceId.length > 0
    readonly property bool isRunning: vm && vm.isSelectedRunning
    readonly property bool canLaunch: hasSelection && !isRunning
    
    // Signals (matching ShellRoot.qml handler names)
    signal editInstance()
    signal changeGroup()
    signal exportInstance()
    signal manageBackups()
    signal copyInstance()
    signal deleteInstance()
    signal createShortcut()
    
    Rectangle {
        anchors.fill: parent
        color: Theme.surface
        border.color: "#323742"
        border.width: 1
        
        ColumnLayout {
            anchors.fill: parent
            anchors.margins: Theme.spacingS
            spacing: 2
            
            // === Launch / Kill Button ===
            ToolButton {
                text: isRunning ? qsTr("Kill") : qsTr("Launch")
                icon.name: isRunning ? "process-stop" : "media-playback-start"
                display: AbstractButton.TextBesideIcon
                Layout.fillWidth: true
                Layout.preferredHeight: 32
                enabled: hasSelection
                
                palette.buttonText: isRunning ? "#ef4444" : (hasSelection ? "#22c55e" : Theme.textSecondary)
                
                onClicked: {
                    if (isRunning && vm) {
                        vm.killSelectedInstance()
                    } else if (vm) {
                        vm.launchSelectedInstance()
                    }
                }
                
                ToolTip.text: isRunning ? qsTr("Kill the running instance") : qsTr("Launch the selected instance")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }
            
            ToolSeparator {
                Layout.fillWidth: true
            }
            
            // === Edit Button ===
            ToolButton {
                text: qsTr("Edit...")
                icon.name: "configure"
                display: AbstractButton.TextBesideIcon
                Layout.fillWidth: true
                Layout.preferredHeight: 28
                enabled: hasSelection
                
                onClicked: instanceToolbar.editInstance()
                
                ToolTip.text: qsTr("Change the instance settings, mods and versions")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }
            
            // === Change Group Button ===
            ToolButton {
                text: qsTr("Change Group...")
                icon.name: "tag"
                display: AbstractButton.TextBesideIcon
                Layout.fillWidth: true
                Layout.preferredHeight: 28
                enabled: hasSelection
                
                onClicked: instanceToolbar.changeGroup()
                
                ToolTip.text: qsTr("Change the selected instance's group")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }
            
            // === Folder Button ===
            ToolButton {
                text: qsTr("Folder")
                icon.name: "folder"
                display: AbstractButton.TextBesideIcon
                Layout.fillWidth: true
                Layout.preferredHeight: 28
                enabled: hasSelection
                
                onClicked: {
                    if (vm) vm.openInstanceFolder()
                }
                
                ToolTip.text: qsTr("Open the selected instance's root folder")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }
            
            // === Export Button ===
            ToolButton {
                text: qsTr("Export...")
                icon.name: "document-export"
                display: AbstractButton.TextBesideIcon
                Layout.fillWidth: true
                Layout.preferredHeight: 28
                enabled: hasSelection && !isRunning
                
                onClicked: instanceToolbar.exportInstance()
                
                ToolTip.text: qsTr("Export the selected instance")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }
            
            // === Manage Backups Button ===
            ToolButton {
                text: qsTr("Backups...")
                icon.name: "document-save"
                display: AbstractButton.TextBesideIcon
                Layout.fillWidth: true
                Layout.preferredHeight: 28
                enabled: hasSelection
                
                onClicked: instanceToolbar.manageBackups()
                
                ToolTip.text: qsTr("Create, restore, and manage instance backups")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }
            
            // === Copy Button ===
            ToolButton {
                text: qsTr("Copy...")
                icon.name: "edit-copy"
                display: AbstractButton.TextBesideIcon
                Layout.fillWidth: true
                Layout.preferredHeight: 28
                enabled: hasSelection && !isRunning
                
                onClicked: instanceToolbar.copyInstance()
                
                ToolTip.text: qsTr("Copy the selected instance")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }
            
            // === Delete Button ===
            ToolButton {
                text: qsTr("Delete")
                icon.name: "edit-delete"
                display: AbstractButton.TextBesideIcon
                Layout.fillWidth: true
                Layout.preferredHeight: 28
                enabled: hasSelection && !isRunning
                
                palette.buttonText: "#ef4444"
                
                onClicked: instanceToolbar.deleteInstance()
                
                ToolTip.text: qsTr("Delete the selected instance")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }
            
            // === Create Shortcut Button ===
            ToolButton {
                text: qsTr("Shortcut")
                icon.name: "link"
                display: AbstractButton.TextBesideIcon
                Layout.fillWidth: true
                Layout.preferredHeight: 28
                enabled: hasSelection
                
                onClicked: instanceToolbar.createShortcut()
                
                ToolTip.text: qsTr("Create a shortcut to launch this instance")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }
            
            // === Spacer ===
            Item { Layout.fillHeight: true }
        }
    }
}
