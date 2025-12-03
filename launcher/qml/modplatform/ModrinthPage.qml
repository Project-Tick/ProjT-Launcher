// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme.js" as Theme

Rectangle {
    id: modrinthPage
    color: Theme.background
    
    property var vm: ProjT.instanceVM
    property string resourceType: "mod"
    
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
                source: "qrc:/icons/multimc/scalable/modrinth.svg"
                fillMode: Image.PreserveAspectFit
            }
            
            Label {
                text: qsTr("Modrinth")
                font.pointSize: 14
                font.bold: true
                color: Theme.textPrimary
            }
            
            Item { Layout.fillWidth: true }
            
            ComboBox {
                id: resourceTypeCombo
                model: [qsTr("Mods"), qsTr("Resource Packs"), qsTr("Shaders"), qsTr("Data Packs"), qsTr("Modpacks")]
                onCurrentIndexChanged: {
                    var types = ["mod", "resourcepack", "shader", "datapack", "modpack"]
                    resourceType = types[currentIndex]
                    if (vm) vm.setModrinthResourceType(resourceType)
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
                placeholderText: qsTr("Search Modrinth...")
                onAccepted: {
                    if (vm) vm.searchModrinth(text)
                }
            }
            
            Button {
                text: qsTr("Search")
                icon.name: "search"
                onClicked: {
                    if (vm) vm.searchModrinth(searchField.text)
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
                model: vm ? vm.modrinthCategories : [qsTr("All Categories")]
                onCurrentIndexChanged: {
                    if (vm) vm.setModrinthCategory(currentIndex)
                }
            }
            
            ComboBox {
                id: loaderCombo
                Layout.preferredWidth: 120
                model: [qsTr("Any Loader"), "Forge", "Fabric", "Quilt", "NeoForge"]
                onCurrentIndexChanged: {
                    if (vm && currentIndex > 0) {
                        vm.setModrinthLoaderFilter(currentText)
                    } else if (vm) {
                        vm.setModrinthLoaderFilter("")
                    }
                }
            }
            
            ComboBox {
                id: sortCombo
                Layout.preferredWidth: 150
                model: [qsTr("Relevance"), qsTr("Downloads"), qsTr("Follows"), qsTr("Newest"), qsTr("Updated")]
                onCurrentIndexChanged: {
                    if (vm) vm.setModrinthSortOrder(currentIndex)
                }
            }
            
            ComboBox {
                id: versionFilterCombo
                Layout.preferredWidth: 130
                model: vm ? vm.availableMinecraftVersions : []
                displayText: currentIndex >= 0 ? currentText : qsTr("Any Version")
                onCurrentIndexChanged: {
                    if (vm && currentIndex >= 0) {
                        vm.setModrinthVersionFilter(currentText)
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
                model: vm ? vm.modrinthResultsModel : []
                
                delegate: ItemDelegate {
                    width: resultsList.width
                    height: 88
                    
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
                                color: "#1bd96a"
                                radius: 8
                                
                                Label {
                                    anchors.centerIn: parent
                                    text: "M"
                                    color: "white"
                                    font.pointSize: 24
                                    font.bold: true
                                }
                            }
                        }
                        
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2
                            
                            RowLayout {
                                spacing: Theme.spacingS
                                
                                Label {
                                    text: model.name || ""
                                    color: Theme.textPrimary
                                    font.bold: true
                                    font.pointSize: 11
                                }
                                
                                // Loaders
                                Repeater {
                                    model: (parent.parent.parent.parent.loaders || "").split(",")
                                    
                                    Rectangle {
                                        visible: modelData.length > 0
                                        width: loaderLabel.width + 8
                                        height: loaderLabel.height + 4
                                        radius: 2
                                        color: Theme.accent
                                        
                                        Label {
                                            id: loaderLabel
                                            anchors.centerIn: parent
                                            text: modelData
                                            color: "white"
                                            font.pointSize: 8
                                        }
                                    }
                                }
                            }
                            
                            Label {
                                text: qsTr("by %1").arg(model.author || qsTr("Unknown"))
                                color: Theme.textSecondary
                                font.pointSize: 9
                            }
                            
                            Label {
                                text: model.description || ""
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
                            
                            RowLayout {
                                spacing: 4
                                
                                Label {
                                    text: "⬇️"
                                    font.pointSize: 9
                                }
                                Label {
                                    text: formatNumber(model.downloads || 0)
                                    color: Theme.textSecondary
                                    font.pointSize: 9
                                }
                            }
                            
                            RowLayout {
                                spacing: 4
                                
                                Label {
                                    text: "❤️"
                                    font.pointSize: 9
                                }
                                Label {
                                    text: formatNumber(model.follows || 0)
                                    color: Theme.textSecondary
                                    font.pointSize: 9
                                }
                            }
                            
                            Button {
                                text: model.installed ? qsTr("Installed") : qsTr("Install")
                                enabled: !model.installed
                                onClicked: {
                                    if (vm) vm.installFromModrinth(model.projectId)
                                }
                            }
                        }
                    }
                }
                
                ScrollBar.vertical: ScrollBar {}
            }
            
            BusyIndicator {
                anchors.centerIn: parent
                running: vm ? vm.modrinthLoading : false
                visible: running
            }
            
            Label {
                anchors.centerIn: parent
                visible: resultsList.count === 0 && !(vm && vm.modrinthLoading)
                text: searchField.text.length > 0 
                      ? qsTr("No results found.")
                      : qsTr("Search for mods, resource packs, shaders, and more on Modrinth.")
                color: Theme.textSecondary
                horizontalAlignment: Text.AlignHCenter
            }
        }
        
        // Pagination
        RowLayout {
            Layout.fillWidth: true
            visible: vm && vm.modrinthTotalPages > 1
            
            Item { Layout.fillWidth: true }
            
            Button {
                text: qsTr("Previous")
                enabled: vm && vm.modrinthCurrentPage > 1
                onClicked: {
                    if (vm) vm.modrinthPreviousPage()
                }
            }
            
            Label {
                text: vm ? qsTr("Page %1 of %2").arg(vm.modrinthCurrentPage).arg(vm.modrinthTotalPages) : ""
                color: Theme.textSecondary
            }
            
            Button {
                text: qsTr("Next")
                enabled: vm && vm.modrinthCurrentPage < vm.modrinthTotalPages
                onClicked: {
                    if (vm) vm.modrinthNextPage()
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
