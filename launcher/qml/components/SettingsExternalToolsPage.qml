// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  External tools settings page
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme.js" as Theme

ScrollView {
    id: externalToolsPage
    clip: true
    
    property var vm: launcherSettingsVM
    
    ColumnLayout {
        width: externalToolsPage.width - Theme.spacingL
        spacing: Theme.spacingM
        
        // Editors
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Editors")
            
            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS
                
                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingS
                    
                    Label {
                        text: qsTr("JSON Editor:")
                        color: Theme.textPrimary
                        Layout.preferredWidth: 100
                    }
                    
                    TextField {
                        Layout.fillWidth: true
                        placeholderText: qsTr("Leave empty to use system default")
                    }
                    
                    Button {
                        text: qsTr("Browse...")
                    }
                }
                
                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingS
                    
                    Label {
                        text: qsTr("Text Editor:")
                        color: Theme.textPrimary
                        Layout.preferredWidth: 100
                    }
                    
                    TextField {
                        Layout.fillWidth: true
                        placeholderText: qsTr("Leave empty to use system default")
                    }
                    
                    Button {
                        text: qsTr("Browse...")
                    }
                }
            }
        }
        
        // JProfiler
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("JProfiler")
            
            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS
                
                CheckBox {
                    text: qsTr("Enable JProfiler integration")
                    checked: false
                }
                
                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingS
                    
                    Label {
                        text: qsTr("Path:")
                        color: Theme.textPrimary
                    }
                    
                    TextField {
                        Layout.fillWidth: true
                        placeholderText: qsTr("/path/to/jprofiler")
                        enabled: false
                    }
                    
                    Button {
                        text: qsTr("Browse...")
                        enabled: false
                    }
                }
            }
        }
        
        // JVisualVM
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("JVisualVM")
            
            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS
                
                CheckBox {
                    text: qsTr("Enable JVisualVM integration")
                    checked: false
                }
                
                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingS
                    
                    Label {
                        text: qsTr("Path:")
                        color: Theme.textPrimary
                    }
                    
                    TextField {
                        Layout.fillWidth: true
                        placeholderText: qsTr("/path/to/jvisualvm")
                        enabled: false
                    }
                    
                    Button {
                        text: qsTr("Browse...")
                        enabled: false
                    }
                }
            }
        }
        
        // MCEdit
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("MCEdit")
            
            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS
                
                CheckBox {
                    text: qsTr("Enable MCEdit integration")
                    checked: false
                }
                
                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingS
                    
                    Label {
                        text: qsTr("Path:")
                        color: Theme.textPrimary
                    }
                    
                    TextField {
                        Layout.fillWidth: true
                        placeholderText: qsTr("/path/to/mcedit")
                        enabled: false
                    }
                    
                    Button {
                        text: qsTr("Browse...")
                        enabled: false
                    }
                }
            }
        }
        
        Item { height: Theme.spacingL }
    }
}
