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
    id: javaPage
    clip: true
    
    property var vm: ProjT.launcherSettingsVM
    
    ColumnLayout {
        width: javaPage.width - Theme.spacingL
        spacing: Theme.spacingM
        
        // Java Runtime
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Java Runtime")
            
            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS
                
                CheckBox {
                    id: autoDetectCheck
                    text: qsTr("Auto-detect Java")
                    checked: vm ? !vm.defaultJavaPath : true
                }
                
                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingS
                    enabled: !autoDetectCheck.checked
                    
                    Label {
                        text: qsTr("Java path:")
                        color: Theme.textPrimary
                    }
                    
                    TextField {
                        id: javaPathField
                        Layout.fillWidth: true
                        placeholderText: qsTr("/usr/bin/java")
                        text: vm ? vm.defaultJavaPath : ""
                        onTextChanged: if (vm && !autoDetectCheck.checked) vm.defaultJavaPath = text
                    }
                    
                    Button {
                        text: qsTr("Browse...")
                        onClicked: javaFileDialog.open()
                    }
                    
                    Button {
                        text: qsTr("Test")
                        onClicked: {
                            // Test Java installation
                            if (vm && vm.testJavaPath) {
                                vm.testJavaPath(javaPathField.text)
                            }
                        }
                    }
                }
                
                Button {
                    text: qsTr("Auto-detect Java installations...")
                    onClicked: {
                        // TODO: Open Java detection dialog
                    }
                }
            }
        }
        
        // Memory
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Memory")
            
            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS
                
                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingM
                    
                    Label {
                        text: qsTr("Minimum memory allocation:")
                        color: Theme.textPrimary
                    }
                    
                    SpinBox {
                        id: minMemSpin
                        from: 256
                        to: 65536
                        value: vm ? vm.defaultMinMemory : 512
                        stepSize: 128
                        editable: true
                        onValueModified: if (vm) vm.defaultMinMemory = value
                    }
                    
                    Label {
                        text: qsTr("MiB")
                        color: Theme.textSecondary
                    }
                }
                
                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingM
                    
                    Label {
                        text: qsTr("Maximum memory allocation:")
                        color: Theme.textPrimary
                    }
                    
                    SpinBox {
                        id: maxMemSpin
                        from: 256
                        to: 65536
                        value: vm ? vm.defaultMaxMemory : 4096
                        stepSize: 128
                        editable: true
                        onValueModified: if (vm) vm.defaultMaxMemory = value
                    }
                    
                    Label {
                        text: qsTr("MiB")
                        color: Theme.textSecondary
                    }
                }
                
                Label {
                    text: qsTr("Note: You generally don't need more than 4-8 GB for Minecraft")
                    color: Theme.textSecondary
                    font.pointSize: 9
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }
            }
        }
        
        // JVM Arguments
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("JVM Arguments")
            
            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS
                
                TextArea {
                    id: jvmArgsArea
                    Layout.fillWidth: true
                    Layout.preferredHeight: 80
                    placeholderText: qsTr("-XX:+UseG1GC -XX:+ParallelRefProcEnabled...")
                    text: vm ? vm.defaultJvmArgs : ""
                    wrapMode: Text.Wrap
                    onTextChanged: if (vm) vm.defaultJvmArgs = text
                }
                
                Label {
                    text: qsTr("Custom JVM arguments. Leave empty for defaults.")
                    color: Theme.textSecondary
                    font.pointSize: 9
                }
            }
        }
        
        Item { height: Theme.spacingL }
    }
    
    FileDialog {
        id: javaFileDialog
        title: qsTr("Select Java Executable")
        nameFilters: ["Java Executable (java java.exe)", "All Files (*)"]
        onAccepted: {
            javaPathField.text = selectedFile.toString().replace("file://", "")
            if (vm) vm.defaultJavaPath = javaPathField.text
        }
    }
}
