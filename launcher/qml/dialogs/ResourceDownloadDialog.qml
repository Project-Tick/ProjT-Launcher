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

Dialog {
    id: resourceDownloadDialog
    title: qsTr("Download Resources")
    modal: true
    width: 600
    height: 500
    standardButtons: Dialog.Cancel
    
    property var vm: null
    property string resourceType: "mod" // mod, resourcepack, shaderpack, world
    property string instanceId: ""
    property string minecraftVersion: ""
    property string loaderType: ""
    
    property string searchQuery: ""
    property var searchResults: []
    property bool searching: false
    
    ColumnLayout {
        anchors.fill: parent
        spacing: Theme.spacingM
        
        // Search bar
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingS
            
            TextField {
                id: searchField
                Layout.fillWidth: true
                placeholderText: qsTr("Search %1...").arg(resourceTypeName())
                text: searchQuery
                onAccepted: performSearch()
            }
            
            Button {
                text: qsTr("Search")
                highlighted: true
                onClicked: performSearch()
            }
        }
        
        // Filters
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingM
            
            ComboBox {
                id: sourceCombo
                Layout.preferredWidth: 140
                model: ["All Sources", "CurseForge", "Modrinth"]
            }
            
            ComboBox {
                id: sortCombo
                Layout.preferredWidth: 140
                model: [
                    qsTr("Relevance"),
                    qsTr("Downloads"),
                    qsTr("Updated"),
                    qsTr("Name")
                ]
            }
            
            Item { Layout.fillWidth: true }
            
            Label {
                text: qsTr("MC %1").arg(minecraftVersion)
                color: ThemeColors.textSecondary
                visible: minecraftVersion.length > 0
            }
            
            Label {
                text: loaderType
                color: ThemeColors.accent
                font.bold: true
                visible: loaderType.length > 0
            }
        }
        
        // Results
        Frame {
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            ListView {
                id: resultsList
                anchors.fill: parent
                clip: true
                model: searchResults
                spacing: 4
                
                delegate: Rectangle {
                    width: resultsList.width
                    height: 80
                    radius: 6
                    color: mouseArea.containsMouse ? ThemeColors.backgroundAlt : "transparent"
                    
                    MouseArea {
                        id: mouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                    }
                    
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: Theme.spacingS
                        spacing: Theme.spacingM
                        
                        // Icon
                        Rectangle {
                            Layout.preferredWidth: 56
                            Layout.preferredHeight: 56
                            radius: 8
                            color: ThemeColors.backgroundAlt
                            
                            Image {
                                anchors.fill: parent
                                anchors.margins: 4
                                source: modelData.iconUrl || ""
                                fillMode: Image.PreserveAspectFit
                            }
                        }
                        
                        ColumnLayout {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            spacing: 2
                            
                            RowLayout {
                                Layout.fillWidth: true
                                
                                Label {
                                    text: modelData.name || qsTr("Unknown")
                                    color: ThemeColors.text
                                    font.bold: true
                                }
                                
                                // Source badge
                                Rectangle {
                                    Layout.preferredWidth: sourceLabel.implicitWidth + 8
                                    Layout.preferredHeight: 16
                                    radius: 8
                                    color: modelData.source === "curseforge" ? ThemeColors.error : "#1bd96a"
                                    
                                    Label {
                                        id: sourceLabel
                                        anchors.centerIn: parent
                                        text: modelData.source === "curseforge" ? "CF" : "MR"
                                        color: "white"
                                        font.pointSize: Theme.fontSizeSmall - 2
                                    }
                                }
                                
                                Item { Layout.fillWidth: true }
                            }
                            
                            Label {
                                Layout.fillWidth: true
                                text: modelData.author || ""
                                color: ThemeColors.textSecondary
                                font.pointSize: Theme.fontSizeSmall
                            }
                            
                            Label {
                                Layout.fillWidth: true
                                text: modelData.description || ""
                                color: ThemeColors.textSecondary
                                font.pointSize: Theme.fontSizeSmall
                                elide: Text.ElideRight
                                maximumLineCount: 2
                                wrapMode: Text.WordWrap
                            }
                            
                            RowLayout {
                                Layout.fillWidth: true
                                spacing: Theme.spacingM
                                
                                Label {
                                    text: qsTr("%1 downloads").arg(formatNumber(modelData.downloads || 0))
                                    color: ThemeColors.textSecondary
                                    font.pointSize: Theme.fontSizeSmall - 1
                                }
                                
                                Item { Layout.fillWidth: true }
                            }
                        }
                        
                        Button {
                            text: modelData.installed ? qsTr("Installed") : qsTr("Install")
                            enabled: !modelData.installed
                            highlighted: !modelData.installed
                            onClicked: {
                                if (vm) {
                                    vm.installResource(modelData.id, modelData.source)
                                }
                            }
                        }
                    }
                }
                
                ScrollBar.vertical: ScrollBar {}
            }
            
            BusyIndicator {
                anchors.centerIn: parent
                running: searching
                visible: running
            }
            
            Label {
                anchors.centerIn: parent
                text: searchQuery.length > 0 ? qsTr("No results found") : qsTr("Enter a search term to find %1").arg(resourceTypeName())
                color: ThemeColors.textSecondary
                visible: !searching && searchResults.length === 0
            }
        }
        
        // Pagination
        RowLayout {
            Layout.fillWidth: true
            visible: searchResults.length > 0
            
            Label {
                text: qsTr("%1 results").arg(searchResults.length)
                color: ThemeColors.textSecondary
            }
            
            Item { Layout.fillWidth: true }
            
            Button {
                text: qsTr("Load More")
                visible: vm ? vm.hasMoreResults : false
                onClicked: {
                    if (vm) vm.loadMoreResults()
                }
            }
        }
    }
    
    function resourceTypeName() {
        switch (resourceType) {
            case "mod": return qsTr("mods")
            case "resourcepack": return qsTr("resource packs")
            case "shaderpack": return qsTr("shader packs")
            case "world": return qsTr("worlds")
            default: return qsTr("resources")
        }
    }
    
    function performSearch() {
        searchQuery = searchField.text
        if (vm && searchQuery.length > 0) {
            searching = true
            vm.searchResources(
                resourceType,
                searchQuery,
                sourceCombo.currentIndex === 0 ? "" : sourceCombo.currentText.toLowerCase(),
                sortCombo.currentIndex
            )
        }
    }
    
    function formatNumber(num) {
        if (num >= 1000000) return (num / 1000000).toFixed(1) + "M"
        if (num >= 1000) return (num / 1000).toFixed(1) + "K"
        return num.toString()
    }
    
    Connections {
        target: vm
        function onSearchComplete(results) {
            searching = false
            searchResults = results
        }
    }
}
