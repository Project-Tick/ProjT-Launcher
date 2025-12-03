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
import "../Theme.js" as Theme

Rectangle {
    id: sidebar
    color: Theme.surface
    
    // ViewModel reference
    readonly property var vm: ProjT.instancesVM
    
    // Selected instance properties (bound to ViewModel)
    readonly property string selectedId: vm ? vm.selectedInstanceId : ""
    readonly property bool hasSelection: selectedId.length > 0
    readonly property bool canLaunch: vm ? vm.canLaunchSelected : false
    readonly property bool isRunning: vm ? vm.isSelectedRunning : false
    
    // Computed instance details
    readonly property int selectedIndex: {
        if (!vm || !vm.instanceIds) return -1
        return vm.instanceIds.indexOf(selectedId)
    }
    readonly property string instanceName: {
        if (selectedIndex < 0 || !vm.instanceNames) return ""
        return vm.instanceNames[selectedIndex] || ""
    }
    readonly property string instanceGroup: {
        if (selectedIndex < 0 || !vm.instanceGroups) return ""
        return vm.instanceGroups[selectedIndex] || ""
    }
    readonly property string iconPath: {
        if (selectedIndex < 0 || !vm.instanceIconPaths) return ""
        return vm.instanceIconPaths[selectedIndex] || ""
    }
    
    // Signals for dialog requests (handled by parent)
    signal renameRequested()
    signal deleteRequested()
    signal duplicateRequested()
    signal editRequested()
    signal createShortcutRequested()
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingM
        spacing: Theme.spacingM
        
        // === Header ===
        Label {
            text: qsTr("Instance Details")
            color: Theme.textSecondary
            font.pointSize: 10
            font.bold: true
            Layout.fillWidth: true
        }
        
        // === Instance Icon ===
        Rectangle {
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: 96
            Layout.preferredHeight: 96
            radius: Theme.radius
            color: hasSelection ? "#3d4d60" : "#2a2d33"
            border.color: "#323742"
            border.width: 1
            
            Image {
                anchors.fill: parent
                anchors.margins: 8
                source: iconPath ? ("file://" + iconPath) : ""
                sourceSize: Qt.size(80, 80)
                fillMode: Image.PreserveAspectFit
                smooth: true
                visible: status === Image.Ready
            }
            
            // Fallback icon placeholder
            Text {
                anchors.centerIn: parent
                text: hasSelection ? instanceName.charAt(0).toUpperCase() : "?"
                font.pointSize: 32
                font.bold: true
                color: Theme.textSecondary
                visible: !hasSelection || iconPath.length === 0
            }
        }
        
        // === Instance Name ===
        Label {
            text: hasSelection ? instanceName : qsTr("No Instance Selected")
            color: Theme.textPrimary
            font.pointSize: 14
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }
        
        // === Instance Group (if any) ===
        Label {
            text: instanceGroup
            color: Theme.textSecondary
            font.pointSize: 10
            horizontalAlignment: Text.AlignHCenter
            visible: instanceGroup.length > 0
            Layout.fillWidth: true
        }
        
        // === Running Status ===
        Rectangle {
            visible: isRunning
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: runningLabel.implicitWidth + 16
            Layout.preferredHeight: 24
            radius: 12
            color: "#2d4a3e"
            border.color: "#4ade80"
            border.width: 1
            
            Label {
                id: runningLabel
                anchors.centerIn: parent
                text: qsTr("Running")
                color: "#4ade80"
                font.pointSize: 9
            }
        }
        
        ToolSeparator {
            Layout.fillWidth: true
        }
        
        // === Action Buttons ===
        ColumnLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingS
            
            // Play / Kill Button
            Button {
                text: isRunning ? qsTr("Kill") : qsTr("Play")
                Layout.fillWidth: true
                Layout.preferredHeight: 40
                enabled: hasSelection && (canLaunch || isRunning)
                highlighted: !isRunning
                
                background: Rectangle {
                    radius: Theme.radius
                    color: {
                        if (!parent.enabled) return "#2a2d33"
                        if (isRunning) return parent.hovered ? "#7f1d1d" : "#991b1b"
                        return parent.hovered ? "#166534" : "#15803d"
                    }
                    border.color: isRunning ? "#ef4444" : "#22c55e"
                    border.width: parent.enabled ? 1 : 0
                }
                
                contentItem: Text {
                    text: parent.text
                    color: parent.enabled ? "#ffffff" : Theme.textSecondary
                    font.pointSize: 12
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                
                onClicked: {
                    if (isRunning && vm) {
                        vm.killSelectedInstance()
                    } else if (vm) {
                        vm.launchSelectedInstance()
                    }
                }
            }
            
            // Edit Settings Button
            Button {
                text: qsTr("Edit")
                Layout.fillWidth: true
                Layout.preferredHeight: 36
                enabled: hasSelection && !isRunning
                
                onClicked: sidebar.editRequested()
                
                ToolTip.text: qsTr("Edit instance settings")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }
            
            // Rename Button
            Button {
                text: qsTr("Rename")
                Layout.fillWidth: true
                Layout.preferredHeight: 36
                enabled: hasSelection && !isRunning
                
                onClicked: sidebar.renameRequested()
                
                ToolTip.text: qsTr("Rename this instance")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }
            
            // Duplicate Button
            Button {
                text: qsTr("Duplicate")
                Layout.fillWidth: true
                Layout.preferredHeight: 36
                enabled: hasSelection && !isRunning
                
                onClicked: sidebar.duplicateRequested()
                
                ToolTip.text: qsTr("Create a copy of this instance")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }
            
            ToolSeparator {
                Layout.fillWidth: true
            }
            
            // Open Folder Button
            Button {
                text: qsTr("Open Folder")
                Layout.fillWidth: true
                Layout.preferredHeight: 36
                enabled: hasSelection
                
                onClicked: {
                    if (vm && vm.openInstanceFolder) {
                        vm.openInstanceFolder(selectedId)
                    }
                }
                
                ToolTip.text: qsTr("Open instance folder in file manager")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }
            
            // Create Shortcut Button
            Button {
                text: qsTr("Create Shortcut")
                Layout.fillWidth: true
                Layout.preferredHeight: 36
                enabled: hasSelection
                
                onClicked: sidebar.createShortcutRequested()
                
                ToolTip.text: qsTr("Create desktop shortcut for this instance")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }
            
            ToolSeparator {
                Layout.fillWidth: true
            }
            
            // Delete Button
            Button {
                text: qsTr("Delete")
                Layout.fillWidth: true
                Layout.preferredHeight: 36
                enabled: hasSelection && !isRunning
                
                background: Rectangle {
                    radius: Theme.radius
                    color: parent.enabled ? (parent.hovered ? "#7f1d1d" : "#1e2227") : "#2a2d33"
                    border.color: parent.enabled ? "#ef4444" : "#323742"
                    border.width: 1
                }
                
                contentItem: Text {
                    text: parent.text
                    color: parent.enabled ? "#ef4444" : Theme.textSecondary
                    font.pointSize: 11
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                
                onClicked: sidebar.deleteRequested()
                
                ToolTip.text: qsTr("Delete this instance")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }
        }
        
        // === Spacer ===
        Item {
            Layout.fillHeight: true
        }
    }
}
