// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme.js" as Theme

Rectangle {
    id: instanceSettingsPage
    color: Theme.background
    
    property var vm: typeof ProjT !== "undefined" && ProjT ? ProjT.instanceVM : null
    
    // Helper functions for safe property access
    function getBool(prop, defaultVal) {
        return vm && typeof vm[prop] !== "undefined" ? vm[prop] : defaultVal
    }
    function getInt(prop, defaultVal) {
        return vm && typeof vm[prop] !== "undefined" ? vm[prop] : defaultVal
    }
    function getString(prop, defaultVal) {
        return vm && typeof vm[prop] !== "undefined" ? vm[prop] : defaultVal
    }
    function setProp(prop, value) {
        if (vm && typeof vm[prop] !== "undefined") vm[prop] = value
    }
    
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
                        checked: getBool("overrideJava", false)
                        onCheckedChanged: setProp("overrideJava", checked)
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
                            text: getString("javaPath", "")
                            placeholderText: qsTr("Auto-detect")
                            onTextChanged: setProp("javaPath", text)
                        }
                        
                        Button {
                            text: qsTr("Browse")
                            onClicked: if (vm && vm.browseJavaPath) vm.browseJavaPath()
                        }
                        
                        Button {
                            text: qsTr("Auto")
                            onClicked: if (vm && vm.autoDetectJava) vm.autoDetectJava()
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
                        checked: getBool("overrideMemory", false)
                        onCheckedChanged: setProp("overrideMemory", checked)
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
                                value: getInt("minMemory", 512)
                                onValueChanged: setProp("minMemory", value)
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
                                value: getInt("maxMemory", 4096)
                                onValueChanged: setProp("maxMemory", value)
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
                        checked: getBool("overrideWindow", false)
                        onCheckedChanged: setProp("overrideWindow", checked)
                    }
                    
                    GridLayout {
                        columns: 4
                        Layout.fillWidth: true
                        enabled: overrideWindowCheck.checked
                        
                        Label { text: qsTr("Width:"); color: Theme.textPrimary }
                        SpinBox {
                            from: 640
                            to: 7680
                            value: getInt("windowWidth", 854)
                            onValueChanged: setProp("windowWidth", value)
                        }
                        
                        Label { text: qsTr("Height:"); color: Theme.textPrimary }
                        SpinBox {
                            from: 480
                            to: 4320
                            value: getInt("windowHeight", 480)
                            onValueChanged: setProp("windowHeight", value)
                        }
                    }
                    
                    CheckBox {
                        text: qsTr("Start maximized")
                        enabled: overrideWindowCheck.checked
                        checked: getBool("startMaximized", false)
                        onCheckedChanged: setProp("startMaximized", checked)
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
                        checked: getBool("overrideJvmArgs", false)
                        onCheckedChanged: setProp("overrideJvmArgs", checked)
                    }
                    
                    TextArea {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 80
                        enabled: overrideArgsCheck.checked
                        text: getString("jvmArgs", "")
                        placeholderText: qsTr("-XX:+UseG1GC -XX:+ParallelRefProcEnabled...")
                        wrapMode: TextEdit.Wrap
                        onTextChanged: setProp("jvmArgs", text)
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
                        checked: getBool("overrideEnv", false)
                        onCheckedChanged: setProp("overrideEnv", checked)
                    }
                    
                    TextArea {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 60
                        enabled: overrideEnvCheck.checked
                        text: getString("envVars", "")
                        placeholderText: qsTr("VAR=value (one per line)")
                        wrapMode: TextEdit.Wrap
                        onTextChanged: setProp("envVars", text)
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
                        checked: getBool("overrideCommands", false)
                        onCheckedChanged: setProp("overrideCommands", checked)
                    }
                    
                    GridLayout {
                        columns: 2
                        Layout.fillWidth: true
                        enabled: overrideCmdsCheck.checked
                        
                        Label { text: qsTr("Pre-launch:"); color: Theme.textPrimary }
                        TextField {
                            Layout.fillWidth: true
                            text: getString("preLaunchCommand", "")
                            onTextChanged: setProp("preLaunchCommand", text)
                        }
                        
                        Label { text: qsTr("Wrapper:"); color: Theme.textPrimary }
                        TextField {
                            Layout.fillWidth: true
                            text: getString("wrapperCommand", "")
                            onTextChanged: setProp("wrapperCommand", text)
                        }
                        
                        Label { text: qsTr("Post-exit:"); color: Theme.textPrimary }
                        TextField {
                            Layout.fillWidth: true
                            text: getString("postExitCommand", "")
                            onTextChanged: setProp("postExitCommand", text)
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
                        checked: getBool("closeOnLaunch", false)
                        onCheckedChanged: setProp("closeOnLaunch", checked)
                    }
                    
                    CheckBox {
                        text: qsTr("Show console while game is running")
                        checked: getBool("showConsole", false)
                        onCheckedChanged: setProp("showConsole", checked)
                    }
                    
                    CheckBox {
                        text: qsTr("Automatically close console when game exits")
                        checked: getBool("autoCloseConsole", true)
                        onCheckedChanged: setProp("autoCloseConsole", checked)
                    }
                }
            }
            
            Item { Layout.preferredHeight: Theme.spacingL }
        }
    }
}
