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

/**
 * ModFilterWidget - Filter sidebar for mod/resource browsing
 */
Rectangle {
    id: modFilterWidget
    color: "transparent"
    
    implicitWidth: 300
    implicitHeight: parent.height
    
    // Filter state
    property var selectedCategories: []
    property var selectedLoaders: []
    property string minVersion: ""
    property string maxVersion: ""
    property int sortBy: 0  // 0=Relevance, 1=Downloads, 2=Recent, 3=Updated
    property int environment: 0  // 0=Any, 1=Client, 2=Server
    property bool showExtendedLoaders: false
    
    signal filtersChanged()
    
    ScrollView {
        anchors.fill: parent
        clip: true
        
        ColumnLayout {
            width: modFilterWidget.width - 16
            spacing: Theme.spacingM
            
            // Categories
            GroupBox {
                Layout.fillWidth: true
                title: qsTr("Categories")
                
                background: Rectangle {
                    y: parent.topPadding - parent.padding
                    width: parent.width
                    height: parent.height - parent.topPadding + parent.padding
                    color: "transparent"
                    border.color: ThemeColors.border
                    radius: Theme.radiusS
                }
                
                label: Label {
                    x: Theme.spacingS
                    text: parent.title
                    color: ThemeColors.text
                }
                
                ColumnLayout {
                    anchors.fill: parent
                    spacing: Theme.spacingXS
                    
                    Repeater {
                        model: ["Adventure", "Decoration", "Equipment", "Food", "Library", 
                               "Magic", "Optimization", "Social", "Storage", "Technology",
                               "Transportation", "Utility", "World Generation"]
                        
                        CheckBox {
                            text: modelData
                            onCheckedChanged: {
                                if (checked) {
                                    selectedCategories.push(modelData)
                                } else {
                                    var idx = selectedCategories.indexOf(modelData)
                                    if (idx >= 0) selectedCategories.splice(idx, 1)
                                }
                                filtersChanged()
                            }
                        }
                    }
                }
            }
            
            // Loaders
            GroupBox {
                Layout.fillWidth: true
                title: qsTr("Loaders")
                
                background: Rectangle {
                    y: parent.topPadding - parent.padding
                    width: parent.width
                    height: parent.height - parent.topPadding + parent.padding
                    color: "transparent"
                    border.color: ThemeColors.border
                    radius: Theme.radiusS
                }
                
                label: Label {
                    x: Theme.spacingS
                    text: parent.title
                    color: ThemeColors.text
                }
                
                ColumnLayout {
                    anchors.fill: parent
                    spacing: Theme.spacingXS
                    
                    // Main loaders
                    CheckBox {
                        text: "NeoForge"
                        onCheckedChanged: updateLoaders("neoforge", checked)
                    }
                    CheckBox {
                        text: "Forge"
                        onCheckedChanged: updateLoaders("forge", checked)
                    }
                    CheckBox {
                        text: "Fabric"
                        onCheckedChanged: updateLoaders("fabric", checked)
                    }
                    CheckBox {
                        text: "Quilt"
                        onCheckedChanged: updateLoaders("quilt", checked)
                    }
                    
                    Button {
                        text: showExtendedLoaders ? qsTr("Show Less") : qsTr("Show More")
                        flat: true
                        onClicked: showExtendedLoaders = !showExtendedLoaders
                    }
                    
                    // Extended loaders
                    ColumnLayout {
                        visible: showExtendedLoaders
                        spacing: Theme.spacingXS
                        
                        CheckBox {
                            text: "LiteLoader"
                            onCheckedChanged: updateLoaders("liteloader", checked)
                        }
                        CheckBox {
                            text: "Babric"
                            onCheckedChanged: updateLoaders("babric", checked)
                        }
                        CheckBox {
                            text: "BTA (Babric)"
                            onCheckedChanged: updateLoaders("btababric", checked)
                        }
                        CheckBox {
                            text: "Legacy Fabric"
                            onCheckedChanged: updateLoaders("legacyfabric", checked)
                        }
                        CheckBox {
                            text: "Ornithe"
                            onCheckedChanged: updateLoaders("ornithe", checked)
                        }
                        CheckBox {
                            text: "Rift"
                            onCheckedChanged: updateLoaders("rift", checked)
                        }
                    }
                }
            }
            
            // Versions
            GroupBox {
                Layout.fillWidth: true
                title: qsTr("Versions")
                
                background: Rectangle {
                    y: parent.topPadding - parent.padding
                    width: parent.width
                    height: parent.height - parent.topPadding + parent.padding
                    color: "transparent"
                    border.color: ThemeColors.border
                    radius: Theme.radiusS
                }
                
                label: Label {
                    x: Theme.spacingS
                    text: parent.title
                    color: ThemeColors.text
                }
                
                GridLayout {
                    anchors.fill: parent
                    columns: 2
                    rowSpacing: Theme.spacingS
                    columnSpacing: Theme.spacingS
                    
                    Label {
                        text: qsTr("Min:")
                        color: ThemeColors.text
                    }
                    
                    ComboBox {
                        Layout.fillWidth: true
                        model: ["Any", "1.21", "1.20.6", "1.20.4", "1.20.1", "1.19.4", "1.18.2", "1.16.5", "1.12.2", "1.7.10"]
                        onCurrentTextChanged: {
                            minVersion = currentText === "Any" ? "" : currentText
                            filtersChanged()
                        }
                    }
                    
                    Label {
                        text: qsTr("Max:")
                        color: ThemeColors.text
                    }
                    
                    ComboBox {
                        Layout.fillWidth: true
                        model: ["Any", "1.21", "1.20.6", "1.20.4", "1.20.1", "1.19.4", "1.18.2", "1.16.5", "1.12.2", "1.7.10"]
                        onCurrentTextChanged: {
                            maxVersion = currentText === "Any" ? "" : currentText
                            filtersChanged()
                        }
                    }
                }
            }
            
            // Sort
            GroupBox {
                Layout.fillWidth: true
                title: qsTr("Sort By")
                
                background: Rectangle {
                    y: parent.topPadding - parent.padding
                    width: parent.width
                    height: parent.height - parent.topPadding + parent.padding
                    color: "transparent"
                    border.color: ThemeColors.border
                    radius: Theme.radiusS
                }
                
                label: Label {
                    x: Theme.spacingS
                    text: parent.title
                    color: ThemeColors.text
                }
                
                ColumnLayout {
                    anchors.fill: parent
                    spacing: Theme.spacingXS
                    
                    RadioButton {
                        text: qsTr("Relevance")
                        checked: sortBy === 0
                        onCheckedChanged: if (checked) { sortBy = 0; filtersChanged() }
                    }
                    RadioButton {
                        text: qsTr("Downloads")
                        checked: sortBy === 1
                        onCheckedChanged: if (checked) { sortBy = 1; filtersChanged() }
                    }
                    RadioButton {
                        text: qsTr("Recently Created")
                        checked: sortBy === 2
                        onCheckedChanged: if (checked) { sortBy = 2; filtersChanged() }
                    }
                    RadioButton {
                        text: qsTr("Recently Updated")
                        checked: sortBy === 3
                        onCheckedChanged: if (checked) { sortBy = 3; filtersChanged() }
                    }
                }
            }
            
            // Environment
            GroupBox {
                Layout.fillWidth: true
                title: qsTr("Environment")
                
                background: Rectangle {
                    y: parent.topPadding - parent.padding
                    width: parent.width
                    height: parent.height - parent.topPadding + parent.padding
                    color: "transparent"
                    border.color: ThemeColors.border
                    radius: Theme.radiusS
                }
                
                label: Label {
                    x: Theme.spacingS
                    text: parent.title
                    color: ThemeColors.text
                }
                
                ColumnLayout {
                    anchors.fill: parent
                    spacing: Theme.spacingXS
                    
                    RadioButton {
                        text: qsTr("Any")
                        checked: environment === 0
                        onCheckedChanged: if (checked) { environment = 0; filtersChanged() }
                    }
                    RadioButton {
                        text: qsTr("Client")
                        checked: environment === 1
                        onCheckedChanged: if (checked) { environment = 1; filtersChanged() }
                    }
                    RadioButton {
                        text: qsTr("Server")
                        checked: environment === 2
                        onCheckedChanged: if (checked) { environment = 2; filtersChanged() }
                    }
                }
            }
            
            // Reset button
            Button {
                Layout.fillWidth: true
                text: qsTr("Reset Filters")
                onClicked: {
                    selectedCategories = []
                    selectedLoaders = []
                    minVersion = ""
                    maxVersion = ""
                    sortBy = 0
                    environment = 0
                    filtersChanged()
                }
            }
            
            Item { Layout.fillHeight: true }
        }
    }
    
    function updateLoaders(loader, checked) {
        if (checked) {
            selectedLoaders.push(loader)
        } else {
            var idx = selectedLoaders.indexOf(loader)
            if (idx >= 0) selectedLoaders.splice(idx, 1)
        }
        filtersChanged()
    }
}
