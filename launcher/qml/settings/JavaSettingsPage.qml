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
    id: javaSettingsPage
    clip: true
    
    property var vm: ProjT.instanceVM
    
    ColumnLayout {
        width: javaSettingsPage.width - Theme.spacingL
        spacing: Theme.spacingM
        
        // Header
        Label {
            text: qsTr("Java Settings")
            color: ThemeColors.text
            font.pointSize: 16
            font.bold: true
        }
        
        Label {
            text: qsTr("Configure Java runtime settings for this instance")
            color: ThemeColors.textSecondary
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }
        
        // Java Override
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Java Runtime")
            
            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS
                
                CheckBox {
                    id: overrideJavaCheck
                    text: qsTr("Override default Java settings")
                    checked: vm ? vm.overrideJava : false
                    onCheckedChanged: {
                        if (vm) vm.overrideJava = checked
                    }
                }
                
                GridLayout {
                    Layout.fillWidth: true
                    columns: 3
                    rowSpacing: Theme.spacingS
                    columnSpacing: Theme.spacingM
                    enabled: overrideJavaCheck.checked
                    opacity: enabled ? 1.0 : 0.5
                    
                    Label {
                        text: qsTr("Java path:")
                        color: ThemeColors.text
                    }
                    
                    TextField {
                        id: javaPathField
                        Layout.fillWidth: true
                        placeholderText: qsTr("/usr/bin/java")
                        text: vm ? vm.javaPath : ""
                        onTextChanged: {
                            if (vm) vm.javaPath = text
                        }
                    }
                    
                    Button {
                        text: qsTr("Browse...")
                        onClicked: browseForJava()
                    }
                    
                    function browseForJava() {
                        if (ProjT.launcherVM && ProjT.launcherVM.browseForFile) {
                            var result = ProjT.launcherVM.browseForFile(qsTr("Select Java Executable"), "")
                            if (result && result.length > 0) {
                                javaPathField.text = result
                                if (vm) vm.javaPath = result
                            }
                        }
                    }
                    
                    Label {
                        text: qsTr("JVM arguments:")
                        color: ThemeColors.text
                    }
                    
                    TextField {
                        id: jvmArgsField
                        Layout.fillWidth: true
                        Layout.columnSpan: 2
                        placeholderText: qsTr("-XX:+UseG1GC -XX:+ParallelRefProcEnabled")
                        text: vm ? vm.jvmArgs : ""
                        onTextChanged: {
                            if (vm) vm.jvmArgs = text
                        }
                    }
                }
            }
        }
        
        // Memory Override
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Memory")
            
            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS
                
                CheckBox {
                    id: overrideMemoryCheck
                    text: qsTr("Override default memory settings")
                    checked: vm ? vm.overrideMemory : false
                    onCheckedChanged: {
                        if (vm) vm.overrideMemory = checked
                    }
                }
                
                GridLayout {
                    Layout.fillWidth: true
                    columns: 3
                    rowSpacing: Theme.spacingS
                    columnSpacing: Theme.spacingM
                    enabled: overrideMemoryCheck.checked
                    opacity: enabled ? 1.0 : 0.5
                    
                    Label {
                        text: qsTr("Minimum memory:")
                        color: ThemeColors.text
                    }
                    
                    SpinBox {
                        id: minMemSpin
                        from: 256
                        to: 65536
                        stepSize: 128
                        value: vm ? vm.minMemory : 512
                        editable: true
                        onValueModified: {
                            if (vm) vm.minMemory = value
                        }
                    }
                    
                    Label {
                        text: qsTr("MiB")
                        color: ThemeColors.textSecondary
                    }
                    
                    Label {
                        text: qsTr("Maximum memory:")
                        color: ThemeColors.text
                    }
                    
                    SpinBox {
                        id: maxMemSpin
                        from: 256
                        to: 65536
                        stepSize: 128
                        value: vm ? vm.maxMemory : 4096
                        editable: true
                        onValueModified: {
                            if (vm) vm.maxMemory = value
                        }
                    }
                    
                    Label {
                        text: qsTr("MiB")
                        color: ThemeColors.textSecondary
                    }
                }
                
                Label {
                    text: qsTr("Note: Most modpacks work well with 4-8 GB of RAM")
                    color: ThemeColors.textSecondary
                    font.pointSize: 9
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }
            }
        }
        
        // Save buttons
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingM
            
            Item { Layout.fillWidth: true }
            
            Button {
                text: qsTr("Reset to Defaults")
                onClicked: {
                    if (vm) {
                        vm.overrideJava = false
                        vm.overrideMemory = false
                        vm.reloadSettings()
                    }
                }
            }
            
            Button {
                text: qsTr("Save")
                highlighted: true
                onClicked: {
                    if (vm) vm.saveSettings()
                }
            }
        }
        
        Item { height: Theme.spacingL }
    }
}
