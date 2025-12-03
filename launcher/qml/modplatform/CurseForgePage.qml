// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme.js" as Theme

Rectangle {
    id: curseForgeModsPage
    color: Theme.background
    
    property var vm: ProjT.instanceVM
    property string resourceType: "mod" // mod, resourcepack, shader, datapack
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingM
        spacing: Theme.spacingS
        
        // Header
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingS
            
            Image {
                Layout.preferredWidth: 24
                Layout.preferredHeight: 24
                source: "qrc:/icons/multimc/scalable/flame.svg"
                fillMode: Image.PreserveAspectFit
            }
            
            Label {
                text: qsTr("CurseForge")
                font.pointSize: 14
                font.bold: true
                color: Theme.textPrimary
            }
            
            Item { Layout.fillWidth: true }
            
            ComboBox {
                id: resourceTypeCombo
                model: [qsTr("Mods"), qsTr("Resource Packs"), qsTr("Shaders")]
                onCurrentIndexChanged: {
                    var types = ["mod", "resourcepack", "shader"]
                    resourceType = types[currentIndex]
                    if (vm) vm.setCurseForgeResourceType(resourceType)
                }
            }
        }
        
        // Search
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingS
            
            TextField {
                id: searchField
                Layout.fillWidth: true
                placeholderText: qsTr("Search CurseForge...")
                onAccepted: {
                    if (vm) vm.searchCurseForge(text)
                }
            }
            
            Button {
                text: qsTr("Search")
                icon.name: "search"
                onClicked: {
                    if (vm) vm.searchCurseForge(searchField.text)
                }
            }
        }
        
        // Filters
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingS
            
            ComboBox {
                id: categoryCombo
                Layout.preferredWidth: 150
                model: vm ? vm.curseForgeCategories : [qsTr("All Categories")]
                onCurrentIndexChanged: {
                    if (vm) vm.setCurseForgeCategory(currentIndex)
                }
            }
            
            ComboBox {
                id: sortCombo
                Layout.preferredWidth: 150
                model: [qsTr("Popularity"), qsTr("Recently Updated"), qsTr("Name"), qsTr("Author"), qsTr("Downloads")]
                onCurrentIndexChanged: {
                    if (vm) vm.setCurseForgeSortOrder(currentIndex)
                }
            }
            
            ComboBox {
                id: versionFilterCombo
                Layout.preferredWidth: 150
                model: vm ? vm.availableMinecraftVersions : []
                displayText: currentIndex >= 0 ? currentText : qsTr("Any Version")
                onCurrentIndexChanged: {
                    if (vm && currentIndex >= 0) {
                        vm.setCurseForgeVersionFilter(currentText)
                    }
                }
            }
            
            Item { Layout.fillWidth: true }
        }
        
        // Results
        Frame {
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            ListView {
                id: resultsList
                anchors.fill: parent
                clip: true
                model: vm ? vm.curseForgeResultsModel : []
                
                delegate: ItemDelegate {
                    width: resultsList.width
                    height: 80
                    
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: Theme.spacingS
                        spacing: Theme.spacingS
                        
                        Image {
                            Layout.preferredWidth: 64
                            Layout.preferredHeight: 64
                            source: model.iconUrl || ""
                            fillMode: Image.PreserveAspectFit
                            
                            Rectangle {
                                anchors.fill: parent
                                visible: parent.status !== Image.Ready
                                color: Theme.surfaceVariant
                                radius: 4
                                
                                Label {
                                    anchors.centerIn: parent
                                    text: "🔥"
                                    font.pointSize: 24
                                }
                            }
                        }
                        
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2
                            
                            Label {
                                text: model.name || ""
                                color: Theme.textPrimary
                                font.bold: true
                                font.pointSize: 11
                                elide: Text.ElideRight
                                Layout.fillWidth: true
                            }
                            
                            Label {
                                text: qsTr("by %1").arg(model.author || qsTr("Unknown"))
                                color: Theme.textSecondary
                                font.pointSize: 9
                            }
                            
                            Label {
                                text: model.summary || ""
                                color: Theme.textSecondary
                                font.pointSize: 9
                                elide: Text.ElideRight
                                Layout.fillWidth: true
                                maximumLineCount: 2
                                wrapMode: Text.WordWrap
                            }
                        }
                        
                        ColumnLayout {
                            spacing: 4
                            
                            Label {
                                text: qsTr("%1 downloads").arg(formatNumber(model.downloads || 0))
                                color: Theme.textSecondary
                                font.pointSize: 9
                            }
                            
                            Button {
                                text: model.installed ? qsTr("Installed") : qsTr("Install")
                                enabled: !model.installed
                                onClicked: {
                                    if (vm) vm.installFromCurseForge(model.projectId)
                                }
                            }
                        }
                    }
                }
                
                ScrollBar.vertical: ScrollBar {}
            }
            
            // Loading indicator
            BusyIndicator {
                anchors.centerIn: parent
                running: vm ? vm.curseForgeLoading : false
                visible: running
            }
            
            Label {
                anchors.centerIn: parent
                visible: resultsList.count === 0 && !(vm && vm.curseForgeLoading)
                text: searchField.text.length > 0 
                      ? qsTr("No results found.")
                      : qsTr("Search for mods, resource packs, and shaders on CurseForge.")
                color: Theme.textSecondary
                horizontalAlignment: Text.AlignHCenter
            }
        }
        
        // Pagination
        RowLayout {
            Layout.fillWidth: true
            visible: vm && vm.curseForgeTotalPages > 1
            
            Item { Layout.fillWidth: true }
            
            Button {
                text: qsTr("Previous")
                enabled: vm && vm.curseForgeCurrentPage > 1
                onClicked: {
                    if (vm) vm.curseForgePreviousPage()
                }
            }
            
            Label {
                text: vm ? qsTr("Page %1 of %2").arg(vm.curseForgeCurrentPage).arg(vm.curseForgeTotalPages) : ""
                color: Theme.textSecondary
            }
            
            Button {
                text: qsTr("Next")
                enabled: vm && vm.curseForgeCurrentPage < vm.curseForgeTotalPages
                onClicked: {
                    if (vm) vm.curseForgeNextPage()
                }
            }
            
            Item { Layout.fillWidth: true }
        }
    }
    
    function formatNumber(num) {
        if (num >= 1000000) return (num / 1000000).toFixed(1) + "M"
        if (num >= 1000) return (num / 1000).toFixed(1) + "K"
        return num.toString()
    }
}
