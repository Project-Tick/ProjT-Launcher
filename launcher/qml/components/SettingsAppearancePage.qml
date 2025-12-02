// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  Appearance settings page
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme.js" as Theme

ScrollView {
    id: appearancePage
    clip: true
    
    property var vm: launcherSettingsVM
    
    property var themeOptions: ["System", "Light", "Dark", "Custom"]
    property var iconThemeOptions: ["Default", "Flat", "Flat (White)", "Legacy", "Simple (Dark)", "Simple (Light)"]
    property var buttonStyleOptions: ["Icon only", "Text only", "Text beside icon", "Text under icon"]
    
    ColumnLayout {
        width: appearancePage.width - Theme.spacingL
        spacing: Theme.spacingM
        
        // Theme
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Theme")
            
            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS
                
                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingM
                    
                    Label {
                        text: qsTr("Application theme:")
                        color: Theme.textPrimary
                    }
                    
                    ComboBox {
                        id: themeCombo
                        Layout.fillWidth: true
                        model: themeOptions
                        currentIndex: vm ? themeOptions.indexOf(vm.theme) : 2
                        onActivated: if (vm) vm.theme = themeOptions[currentIndex]
                    }
                }
                
                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingM
                    
                    Label {
                        text: qsTr("Icon theme:")
                        color: Theme.textPrimary
                    }
                    
                    ComboBox {
                        id: iconThemeCombo
                        Layout.fillWidth: true
                        model: iconThemeOptions
                        currentIndex: vm ? iconThemeOptions.indexOf(vm.iconTheme) : 0
                        onActivated: if (vm) vm.iconTheme = iconThemeOptions[currentIndex]
                    }
                }
            }
        }
        
        // Toolbar
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Toolbar")
            
            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS
                
                CheckBox {
                    text: qsTr("Show toolbar text labels")
                    checked: vm ? vm.showToolbarText : true
                    onCheckedChanged: if (vm) vm.showToolbarText = checked
                }
                
                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingM
                    
                    Label {
                        text: qsTr("Button style:")
                        color: Theme.textPrimary
                    }
                    
                    ComboBox {
                        id: buttonStyleCombo
                        Layout.fillWidth: true
                        model: buttonStyleOptions
                        currentIndex: 3
                        // TODO: bind to vm.buttonStyle when added
                    }
                }
            }
        }
        
        // Instance list
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Instance List")
            
            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS
                
                CheckBox {
                    text: qsTr("Show instance icons")
                    checked: vm ? vm.instanceListIcons : true
                    onCheckedChanged: if (vm) vm.instanceListIcons = checked
                }
                
                CheckBox {
                    text: qsTr("Show instance status light")
                    checked: vm ? vm.showInstanceStatusLight : true
                    onCheckedChanged: if (vm) vm.showInstanceStatusLight = checked
                }
                
                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingM
                    
                    Label {
                        text: qsTr("Icon size:")
                        color: Theme.textPrimary
                    }
                    
                    Slider {
                        id: iconSizeSlider
                        Layout.fillWidth: true
                        from: 32
                        to: 128
                        value: 48
                        stepSize: 8
                    }
                    
                    Label {
                        text: iconSizeSlider.value + "px"
                        color: Theme.textSecondary
                    }
                }
            }
        }
        
        // Cat
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Cat")
            
            CheckBox {
                text: qsTr("Catify the launcher")
                checked: vm ? vm.enableCat : false
                onCheckedChanged: if (vm) vm.enableCat = checked
            }
        }
        
        Item { height: Theme.spacingL }
    }
}
