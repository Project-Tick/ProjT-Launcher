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
import "../Theme.js" as Theme

ScrollView {
    id: externalToolsPage
    clip: true
    
    property var vm: ProjT.launcherSettingsVM
    
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
                        id: jsonEditorField
                        Layout.fillWidth: true
                        placeholderText: qsTr("Leave empty to use system default")
                        text: vm ? vm.jsonEditorPath : ""
                        onTextChanged: if (vm) vm.jsonEditorPath = text
                    }
                    
                    Button {
                        text: qsTr("Browse...")
                        onClicked: browseForJsonEditor()
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
                    id: jprofilerCheck
                    text: qsTr("Enable JProfiler integration")
                    checked: vm ? vm.jprofilerPath.length > 0 : false
                }
                
                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingS
                    enabled: jprofilerCheck.checked
                    
                    Label {
                        text: qsTr("Path:")
                        color: Theme.textPrimary
                    }
                    
                    TextField {
                        id: jprofilerField
                        Layout.fillWidth: true
                        placeholderText: qsTr("/path/to/jprofiler")
                        text: vm ? vm.jprofilerPath : ""
                        onTextChanged: if (vm) vm.jprofilerPath = text
                    }
                    
                    Button {
                        text: qsTr("Browse...")
                        onClicked: browseForJProfiler()
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
                    id: jvisualvmCheck
                    text: qsTr("Enable JVisualVM integration")
                    checked: vm ? vm.jvisualvmPath.length > 0 : false
                }
                
                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingS
                    enabled: jvisualvmCheck.checked
                    
                    Label {
                        text: qsTr("Path:")
                        color: Theme.textPrimary
                    }
                    
                    TextField {
                        id: jvisualvmField
                        Layout.fillWidth: true
                        placeholderText: qsTr("/path/to/jvisualvm")
                        text: vm ? vm.jvisualvmPath : ""
                        onTextChanged: if (vm) vm.jvisualvmPath = text
                    }
                    
                    Button {
                        text: qsTr("Browse...")
                        onClicked: browseForJVisualVM()
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
                    id: mceditCheck
                    text: qsTr("Enable MCEdit integration")
                    checked: vm ? vm.mceditPath.length > 0 : false
                }
                
                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingS
                    enabled: mceditCheck.checked
                    
                    Label {
                        text: qsTr("Path:")
                        color: Theme.textPrimary
                    }
                    
                    TextField {
                        id: mceditField
                        Layout.fillWidth: true
                        placeholderText: qsTr("/path/to/mcedit")
                        text: vm ? vm.mceditPath : ""
                        onTextChanged: if (vm) vm.mceditPath = text
                    }
                    
                    Button {
                        text: qsTr("Browse...")
                        onClicked: browseForMCEdit()
                    }
                }
            }
        }
        
        Item { height: Theme.spacingL }
    }
    
    // Browse functions (replaces FileDialog for Qt 6 compatibility)
    function browseForJsonEditor() {
        if (ProjT.launcherVM && ProjT.launcherVM.browseForFile) {
            var result = ProjT.launcherVM.browseForFile(qsTr("Select JSON Editor"), "")
            if (result && result.length > 0) {
                jsonEditorField.text = result
                if (vm) vm.jsonEditorPath = result
            }
        }
    }
    
    function browseForJProfiler() {
        if (ProjT.launcherVM && ProjT.launcherVM.browseForFile) {
            var result = ProjT.launcherVM.browseForFile(qsTr("Select JProfiler Executable"), "")
            if (result && result.length > 0) {
                jprofilerField.text = result
                if (vm) vm.jprofilerPath = result
            }
        }
    }
    
    function browseForJVisualVM() {
        if (ProjT.launcherVM && ProjT.launcherVM.browseForFile) {
            var result = ProjT.launcherVM.browseForFile(qsTr("Select JVisualVM Executable"), "")
            if (result && result.length > 0) {
                jvisualvmField.text = result
                if (vm) vm.jvisualvmPath = result
            }
        }
    }
    
    function browseForMCEdit() {
        if (ProjT.launcherVM && ProjT.launcherVM.browseForFile) {
            var result = ProjT.launcherVM.browseForFile(qsTr("Select MCEdit Executable"), "")
            if (result && result.length > 0) {
                mceditField.text = result
                if (vm) vm.mceditPath = result
            }
        }
    }
}
