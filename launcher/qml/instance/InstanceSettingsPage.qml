// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme.js" as Theme

Rectangle {
    id: instanceSettingsPage
    color: Theme.background
    
    property var vm: ProjT.instanceVM
    
    ScrollView {
        anchors.fill: parent
        anchors.margins: Theme.spacingM
        clip: true
        
        ColumnLayout {
            width: parent.width
            spacing: Theme.spacingM
            
            Label {
                text: qsTr("Instance Settings")
                font.pointSize: 14
                font.bold: true
                color: Theme.textPrimary
            }
            
            // Java Settings
            GroupBox {
                Layout.fillWidth: true
                title: qsTr("Java")
                
                ColumnLayout {
                    anchors.fill: parent
                    spacing: Theme.spacingS
                    
                    CheckBox {
                        id: overrideJavaCheck
                        text: qsTr("Override global Java settings")
                        checked: vm ? vm.overrideJava : false
                        onCheckedChanged: {
                            if (vm) vm.overrideJava = checked
                        }
                    }
                    
                    RowLayout {
                        Layout.fillWidth: true
                        enabled: overrideJavaCheck.checked
                        
                        Label {
                            text: qsTr("Java Path:")
                            color: Theme.textPrimary
                        }
                        
                        TextField {
                            Layout.fillWidth: true
                            text: vm ? vm.javaPath : ""
                            placeholderText: qsTr("Auto-detect")
                            onTextChanged: {
                                if (vm) vm.javaPath = text
                            }
                        }
                        
                        Button {
                            text: qsTr("Browse")
                            onClicked: {
                                if (vm) vm.browseJavaPath()
                            }
                        }
                        
                        Button {
                            text: qsTr("Auto")
                            onClicked: {
                                if (vm) vm.autoDetectJava()
                            }
                        }
                    }
                }
            }
            
            // Memory Settings
            GroupBox {
                Layout.fillWidth: true
                title: qsTr("Memory")
                
                ColumnLayout {
                    anchors.fill: parent
                    spacing: Theme.spacingS
                    
                    CheckBox {
                        id: overrideMemoryCheck
                        text: qsTr("Override global memory settings")
                        checked: vm ? vm.overrideMemory : false
                        onCheckedChanged: {
                            if (vm) vm.overrideMemory = checked
                        }
                    }
                    
                    GridLayout {
                        columns: 2
                        Layout.fillWidth: true
                        enabled: overrideMemoryCheck.checked
                        
                        Label {
                            text: qsTr("Minimum:")
                            color: Theme.textPrimary
                        }
                        
                        RowLayout {
                            SpinBox {
                                id: minMemSpin
                                from: 256
                                to: 65536
                                stepSize: 128
                                value: vm ? vm.minMemory : 512
                                onValueChanged: {
                                    if (vm) vm.minMemory = value
                                }
                            }
                            Label {
                                text: qsTr("MB")
                                color: Theme.textSecondary
                            }
                        }
                        
                        Label {
                            text: qsTr("Maximum:")
                            color: Theme.textPrimary
                        }
                        
                        RowLayout {
                            SpinBox {
                                id: maxMemSpin
                                from: 256
                                to: 65536
                                stepSize: 128
                                value: vm ? vm.maxMemory : 4096
                                onValueChanged: {
                                    if (vm) vm.maxMemory = value
                                }
                            }
                            Label {
                                text: qsTr("MB")
                                color: Theme.textSecondary
                            }
                        }
                    }
                    
                    Slider {
                        Layout.fillWidth: true
                        enabled: overrideMemoryCheck.checked
                        from: 256
                        to: 16384
                        stepSize: 128
                        value: maxMemSpin.value
                        onValueChanged: maxMemSpin.value = value
                    }
                }
            }
            
            // Game Window
            GroupBox {
                Layout.fillWidth: true
                title: qsTr("Game Window")
                
                ColumnLayout {
                    anchors.fill: parent
                    spacing: Theme.spacingS
                    
                    CheckBox {
                        id: overrideWindowCheck
                        text: qsTr("Override global window settings")
                        checked: vm ? vm.overrideWindow : false
                        onCheckedChanged: {
                            if (vm) vm.overrideWindow = checked
                        }
                    }
                    
                    GridLayout {
                        columns: 4
                        Layout.fillWidth: true
                        enabled: overrideWindowCheck.checked
                        
                        Label { text: qsTr("Width:"); color: Theme.textPrimary }
                        SpinBox {
                            from: 640
                            to: 7680
                            value: vm ? vm.windowWidth : 854
                            onValueChanged: {
                                if (vm) vm.windowWidth = value
                            }
                        }
                        
                        Label { text: qsTr("Height:"); color: Theme.textPrimary }
                        SpinBox {
                            from: 480
                            to: 4320
                            value: vm ? vm.windowHeight : 480
                            onValueChanged: {
                                if (vm) vm.windowHeight = value
                            }
                        }
                    }
                    
                    CheckBox {
                        text: qsTr("Start maximized")
                        enabled: overrideWindowCheck.checked
                        checked: vm ? vm.startMaximized : false
                        onCheckedChanged: {
                            if (vm) vm.startMaximized = checked
                        }
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
                    
                    CheckBox {
                        id: overrideArgsCheck
                        text: qsTr("Override global JVM arguments")
                        checked: vm ? vm.overrideJvmArgs : false
                        onCheckedChanged: {
                            if (vm) vm.overrideJvmArgs = checked
                        }
                    }
                    
                    TextArea {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 80
                        enabled: overrideArgsCheck.checked
                        text: vm ? vm.jvmArgs : ""
                        placeholderText: qsTr("-XX:+UseG1GC -XX:+ParallelRefProcEnabled...")
                        wrapMode: TextEdit.Wrap
                        onTextChanged: {
                            if (vm) vm.jvmArgs = text
                        }
                    }
                }
            }
            
            // Environment Variables
            GroupBox {
                Layout.fillWidth: true
                title: qsTr("Environment Variables")
                
                ColumnLayout {
                    anchors.fill: parent
                    spacing: Theme.spacingS
                    
                    CheckBox {
                        id: overrideEnvCheck
                        text: qsTr("Override global environment variables")
                        checked: vm ? vm.overrideEnv : false
                        onCheckedChanged: {
                            if (vm) vm.overrideEnv = checked
                        }
                    }
                    
                    TextArea {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 60
                        enabled: overrideEnvCheck.checked
                        text: vm ? vm.envVars : ""
                        placeholderText: qsTr("VAR=value (one per line)")
                        wrapMode: TextEdit.Wrap
                        onTextChanged: {
                            if (vm) vm.envVars = text
                        }
                    }
                }
            }
            
            // Commands
            GroupBox {
                Layout.fillWidth: true
                title: qsTr("Custom Commands")
                
                ColumnLayout {
                    anchors.fill: parent
                    spacing: Theme.spacingS
                    
                    CheckBox {
                        id: overrideCmdsCheck
                        text: qsTr("Override global custom commands")
                        checked: vm ? vm.overrideCommands : false
                        onCheckedChanged: {
                            if (vm) vm.overrideCommands = checked
                        }
                    }
                    
                    GridLayout {
                        columns: 2
                        Layout.fillWidth: true
                        enabled: overrideCmdsCheck.checked
                        
                        Label { text: qsTr("Pre-launch:"); color: Theme.textPrimary }
                        TextField {
                            Layout.fillWidth: true
                            text: vm ? vm.preLaunchCommand : ""
                            onTextChanged: {
                                if (vm) vm.preLaunchCommand = text
                            }
                        }
                        
                        Label { text: qsTr("Wrapper:"); color: Theme.textPrimary }
                        TextField {
                            Layout.fillWidth: true
                            text: vm ? vm.wrapperCommand : ""
                            onTextChanged: {
                                if (vm) vm.wrapperCommand = text
                            }
                        }
                        
                        Label { text: qsTr("Post-exit:"); color: Theme.textPrimary }
                        TextField {
                            Layout.fillWidth: true
                            text: vm ? vm.postExitCommand : ""
                            onTextChanged: {
                                if (vm) vm.postExitCommand = text
                            }
                        }
                    }
                }
            }
            
            // Miscellaneous
            GroupBox {
                Layout.fillWidth: true
                title: qsTr("Miscellaneous")
                
                ColumnLayout {
                    anchors.fill: parent
                    spacing: Theme.spacingS
                    
                    CheckBox {
                        text: qsTr("Close launcher when game starts")
                        checked: vm ? vm.closeOnLaunch : false
                        onCheckedChanged: {
                            if (vm) vm.closeOnLaunch = checked
                        }
                    }
                    
                    CheckBox {
                        text: qsTr("Show console while game is running")
                        checked: vm ? vm.showConsole : false
                        onCheckedChanged: {
                            if (vm) vm.showConsole = checked
                        }
                    }
                    
                    CheckBox {
                        text: qsTr("Automatically close console when game exits")
                        checked: vm ? vm.autoCloseConsole : true
                        onCheckedChanged: {
                            if (vm) vm.autoCloseConsole = checked
                        }
                    }
                }
            }
            
            Item { Layout.preferredHeight: Theme.spacingL }
        }
    }
}
