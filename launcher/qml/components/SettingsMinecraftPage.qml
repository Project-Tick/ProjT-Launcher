// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  Minecraft settings page
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme.js" as Theme

ScrollView {
    id: minecraftPage
    clip: true
    
    property var vm: launcherSettingsVM
    
    ColumnLayout {
        width: minecraftPage.width - Theme.spacingL
        spacing: Theme.spacingM
        
        // Window
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Window")
            
            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS
                
                CheckBox {
                    id: maximizedCheck
                    text: qsTr("Start Minecraft maximized")
                    checked: false
                }
                
                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingM
                    
                    Label {
                        text: qsTr("Window width:")
                        color: Theme.textPrimary
                    }
                    
                    SpinBox {
                        id: windowWidthSpin
                        from: 256
                        to: 4096
                        value: 854
                        editable: true
                    }
                    
                    Label {
                        text: qsTr("Window height:")
                        color: Theme.textPrimary
                    }
                    
                    SpinBox {
                        id: windowHeightSpin
                        from: 256
                        to: 4096
                        value: 480
                        editable: true
                    }
                }
            }
        }
        
        // Game Time
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Game Time")
            
            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS
                
                CheckBox {
                    text: qsTr("Record time spent playing")
                    checked: vm ? vm.showGameTime : true
                    onCheckedChanged: if (vm) vm.showGameTime = checked
                }
                
                CheckBox {
                    text: qsTr("Show time spent playing in the instance list")
                    checked: vm ? vm.showGlobalGameTime : true
                    onCheckedChanged: if (vm) vm.showGlobalGameTime = checked
                }
            }
        }
        
        // Mods Management
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Mods Management")
            
            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS
                
                CheckBox {
                    text: qsTr("Enable \"Manage Mods\" button")
                    checked: vm ? vm.enableManageModsButton : true
                    onCheckedChanged: if (vm) vm.enableManageModsButton = checked
                }
            }
        }
        
        // Linux Specific
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Linux Specific")
            visible: Qt.platform.os === "linux"
            
            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS
                
                CheckBox {
                    text: qsTr("Enable Feral GameMode")
                    checked: vm ? vm.enableFeralGamemode : false
                    onCheckedChanged: if (vm) vm.enableFeralGamemode = checked
                }
                
                CheckBox {
                    text: qsTr("Use discrete GPU (prime-run)")
                    checked: vm ? vm.enableDiscreteGpu : false
                    onCheckedChanged: if (vm) vm.enableDiscreteGpu = checked
                }
                
                CheckBox {
                    text: qsTr("Use MangoHud")
                    checked: vm ? vm.enableMangoHud : false
                    onCheckedChanged: if (vm) vm.enableMangoHud = checked
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
                    text: qsTr("Show game log in console")
                    checked: true
                }
                
                CheckBox {
                    text: qsTr("Skip Mojang account migration check")
                    checked: false
                }
                
                CheckBox {
                    text: qsTr("Use native OpenAL")
                    checked: false
                }
                
                CheckBox {
                    text: qsTr("Use native GLFW")
                    checked: false
                }
            }
        }
        
        Item { height: Theme.spacingL }
    }
}
