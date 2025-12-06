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
import "../../Theme.js" as Theme

Rectangle {
    id: importFTBPage
    color: ThemeColors.background
    
    property var vm: ProjT.instancesVM
    property string searchText: ""
    property string ftbPath: ""
    
    signal instanceSelected(string instanceId)
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingM
        spacing: Theme.spacingS
        
        // Note label
        Label {
            Layout.fillWidth: true
            text: qsTr("Note: If your FTB instances are not in the default location, select it using the button next to search.")
            color: ThemeColors.textSecondary
            font.italic: true
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
        }
        
        // Search bar
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingS
            
            TextField {
                id: searchField
                Layout.fillWidth: true
                placeholderText: qsTr("Search and filter...")
                text: searchText
                onTextChanged: {
                    searchText = text
                    if (vm) vm.filterFTBInstances(text)
                }
            }
            
            ToolButton {
                icon.name: "folder-open"
                ToolTip.visible: hovered
                ToolTip.text: qsTr("Select FTBApp instances directory")
                onClicked: {
                    if (vm) vm.browseFTBInstancesDir()
                }
            }
        }
        
        // Instance list
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: ThemeColors.backgroundAlt
            border.color: ThemeColors.border
            radius: Theme.radiusS
            
            TreeView {
                id: modpackTree
                anchors.fill: parent
                anchors.margins: 1
                clip: true
                model: vm ? vm.ftbInstancesModel : null
                
                delegate: ItemDelegate {
                    width: modpackTree.width
                    height: 50
                    
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: Theme.spacingS
                        spacing: Theme.spacingM
                        
                        Image {
                            width: 32
                            height: 32
                            source: model.iconUrl || ""
                            fillMode: Image.PreserveAspectFit
                        }
                        
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2
                            
                            Label {
                                text: model.name || ""
                                color: ThemeColors.text
                                font.bold: true
                            }
                            
                            Label {
                                text: model.version || ""
                                color: ThemeColors.textSecondary
                                font.pixelSize: 11
                            }
                        }
                    }
                    
                    onClicked: instanceSelected(model.id)
                    onDoubleClicked: {
                        instanceSelected(model.id)
                        if (vm) vm.importFTBInstance(model.id)
                    }
                }
                
                ScrollBar.vertical: ScrollBar {}
            }
            
            Label {
                anchors.centerIn: parent
                visible: modpackTree.model === null || modpackTree.model.count === 0
                text: qsTr("No FTB instances found")
                color: ThemeColors.textSecondary
            }
        }
        
        // Sort options
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingS
            
            ComboBox {
                id: sortByBox
                Layout.minimumWidth: 265
                model: [qsTr("Sort by Name"), qsTr("Sort by Last Played"), qsTr("Sort by Game Version")]
                onCurrentIndexChanged: {
                    if (vm) vm.sortFTBInstances(currentIndex)
                }
            }
            
            Item { Layout.fillWidth: true }
            
            Button {
                text: qsTr("Import Selected")
                enabled: modpackTree.currentIndex >= 0
                onClicked: {
                    if (vm && modpackTree.currentIndex >= 0) {
                        vm.importFTBInstance(vm.ftbInstancesModel[modpackTree.currentIndex].id)
                    }
                }
            }
        }
    }
    
    Component.onCompleted: {
        if (vm) vm.loadFTBInstances()
    }
    
    Connections {
        target: vm
        ignoreUnknownSignals: true
        function onFtbPathSelected(path) {
            ftbPath = path
        }
    }
}
