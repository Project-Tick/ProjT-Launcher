// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme.js" as Theme

Page {
    id: technicPage
    title: qsTr("Technic")
    
    property var vm: ProjT ? ProjT.technicVM : null
    property string searchQuery: ""
    property var searchResults: []
    property bool isSearching: false
    property string currentTab: "trending"
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingM
        spacing: Theme.spacingM
        
        // Header
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingM
            
            Rectangle {
                Layout.preferredWidth: 48
                Layout.preferredHeight: 48
                radius: 8
                color: "#2c3e50"
                
                Label {
                    anchors.centerIn: parent
                    text: "T"
                    color: "#e74c3c"
                    font.bold: true
                    font.pointSize: 18
                }
            }
            
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 2
                
                Label {
                    text: qsTr("Technic Platform")
                    color: Theme.textPrimary
                    font.bold: true
                    font.pointSize: Theme.fontSizeMedium
                }
                
                Label {
                    text: qsTr("Browse Technic modpacks and Solder packs")
                    color: Theme.textSecondary
                    font.pointSize: Theme.fontSizeSmall
                }
            }
        }
        
        // Tab bar
        TabBar {
            id: tabBar
            Layout.fillWidth: true
            
            TabButton {
                text: qsTr("Trending")
                onClicked: currentTab = "trending"
            }
            
            TabButton {
                text: qsTr("Search")
                onClicked: currentTab = "search"
            }
            
            TabButton {
                text: qsTr("Import Solder")
                onClicked: currentTab = "solder"
            }
        }
        
        // Search (visible in search tab)
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingS
            visible: currentTab === "search"
            
            TextField {
                id: searchField
                Layout.fillWidth: true
                placeholderText: qsTr("Search Technic modpacks...")
                onAccepted: performSearch()
            }
            
            Button {
                text: qsTr("Search")
                highlighted: true
                onClicked: performSearch()
            }
        }
        
        // Solder URL input (visible in solder tab)
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Import from Solder URL")
            visible: currentTab === "solder"
            
            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS
                
                Label {
                    Layout.fillWidth: true
                    text: qsTr("Enter a Technic Solder pack URL or slug to import:")
                    color: Theme.textSecondary
                    wrapMode: Text.WordWrap
                }
                
                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingS
                    
                    TextField {
                        id: solderUrlField
                        Layout.fillWidth: true
                        placeholderText: qsTr("https://technicpack.net/modpack/... or pack-slug")
                    }
                    
                    Button {
                        text: qsTr("Import")
                        highlighted: true
                        enabled: solderUrlField.text.length > 0
                        onClicked: {
                            if (vm) vm.importFromSolder(solderUrlField.text)
                        }
                    }
                }
            }
        }
        
        // Content
        Frame {
            Layout.fillWidth: true
            Layout.fillHeight: true
            visible: currentTab !== "solder"
            
            GridView {
                id: packGrid
                anchors.fill: parent
                clip: true
                cellWidth: 200
                cellHeight: 250
                model: {
                    if (currentTab === "search") return searchResults
                    return vm ? vm.trendingPacks : []
                }
                
                delegate: Rectangle {
                    width: packGrid.cellWidth - 8
                    height: packGrid.cellHeight - 8
                    radius: 8
                    color: mouseArea.containsMouse ? Theme.backgroundAlt : "transparent"
                    border.color: mouseArea.containsMouse ? Theme.accent : Theme.divider
                    border.width: 1
                    
                    MouseArea {
                        id: mouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            detailDialog.pack = modelData
                            detailDialog.open()
                        }
                    }
                    
                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: Theme.spacingS
                        spacing: Theme.spacingS
                        
                        // Pack logo
                        Rectangle {
                            Layout.alignment: Qt.AlignHCenter
                            Layout.preferredWidth: 120
                            Layout.preferredHeight: 72
                            radius: 4
                            color: Theme.backgroundAlt
                            
                            Image {
                                anchors.fill: parent
                                anchors.margins: 2
                                source: modelData.logoUrl || ""
                                fillMode: Image.PreserveAspectFit
                            }
                        }
                        
                        // Pack name
                        Label {
                            Layout.fillWidth: true
                            text: modelData.name || qsTr("Unknown")
                            color: Theme.textPrimary
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            elide: Text.ElideRight
                            maximumLineCount: 2
                            wrapMode: Text.WordWrap
                        }
                        
                        // Stats
                        RowLayout {
                            Layout.alignment: Qt.AlignHCenter
                            spacing: Theme.spacingM
                            
                            RowLayout {
                                spacing: 2
                                
                                Label {
                                    text: "👁"
                                    font.pointSize: Theme.fontSizeSmall - 1
                                }
                                
                                Label {
                                    text: formatNumber(modelData.runs || 0)
                                    color: Theme.textSecondary
                                    font.pointSize: Theme.fontSizeSmall
                                }
                            }
                            
                            RowLayout {
                                spacing: 2
                                
                                Label {
                                    text: "⬇"
                                    font.pointSize: Theme.fontSizeSmall - 1
                                }
                                
                                Label {
                                    text: formatNumber(modelData.downloads || 0)
                                    color: Theme.textSecondary
                                    font.pointSize: Theme.fontSizeSmall
                                }
                            }
                        }
                        
                        // Rating
                        RowLayout {
                            Layout.alignment: Qt.AlignHCenter
                            spacing: 2
                            
                            Repeater {
                                model: 5
                                Label {
                                    text: index < Math.round(modelData.rating || 0) ? "★" : "☆"
                                    color: index < Math.round(modelData.rating || 0) ? "#f59e0b" : Theme.textSecondary
                                    font.pointSize: Theme.fontSizeSmall
                                }
                            }
                        }
                        
                        Item { Layout.fillHeight: true }
                        
                        // Install button
                        Button {
                            Layout.fillWidth: true
                            text: qsTr("Install")
                            highlighted: true
                            onClicked: {
                                if (vm) vm.installPack(modelData.slug)
                            }
                        }
                    }
                }
                
                ScrollBar.vertical: ScrollBar {}
            }
            
            BusyIndicator {
                anchors.centerIn: parent
                running: isSearching || (vm ? vm.loading : false)
                visible: running
            }
            
            Label {
                anchors.centerIn: parent
                text: currentTab === "search" ? qsTr("Enter a search term") : qsTr("No modpacks found")
                color: Theme.textSecondary
                visible: !isSearching && packGrid.count === 0
            }
        }
    }
    
    function performSearch() {
        searchQuery = searchField.text
        if (vm && searchQuery.length > 0) {
            isSearching = true
            vm.search(searchQuery)
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
            isSearching = false
            searchResults = results
        }
    }
    
    // Pack detail dialog
    Dialog {
        id: detailDialog
        title: pack ? pack.name : ""
        modal: true
        width: 550
        height: 500
        standardButtons: Dialog.Close
        
        property var pack: null
        
        ColumnLayout {
            anchors.fill: parent
            spacing: Theme.spacingM
            
            // Header with background
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 120
                radius: 8
                color: Theme.backgroundAlt
                
                Image {
                    anchors.fill: parent
                    source: detailDialog.pack ? detailDialog.pack.backgroundUrl : ""
                    fillMode: Image.PreserveAspectCrop
                    opacity: 0.3
                }
                
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: Theme.spacingM
                    spacing: Theme.spacingM
                    
                    Rectangle {
                        Layout.preferredWidth: 96
                        Layout.preferredHeight: 96
                        radius: 8
                        color: Theme.background
                        
                        Image {
                            anchors.fill: parent
                            anchors.margins: 4
                            source: detailDialog.pack ? detailDialog.pack.logoUrl : ""
                            fillMode: Image.PreserveAspectFit
                        }
                    }
                    
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 4
                        
                        Label {
                            text: detailDialog.pack ? detailDialog.pack.name : ""
                            color: Theme.textPrimary
                            font.bold: true
                            font.pointSize: Theme.fontSizeMedium
                        }
                        
                        Label {
                            text: qsTr("by %1").arg(detailDialog.pack ? detailDialog.pack.user : "")
                            color: Theme.textSecondary
                        }
                        
                        RowLayout {
                            spacing: Theme.spacingM
                            
                            Label {
                                text: qsTr("%1 downloads").arg(formatNumber(detailDialog.pack ? detailDialog.pack.downloads : 0))
                                color: Theme.accent
                            }
                            
                            RowLayout {
                                spacing: 2
                                Repeater {
                                    model: 5
                                    Label {
                                        text: index < Math.round(detailDialog.pack ? detailDialog.pack.rating : 0) ? "★" : "☆"
                                        color: "#f59e0b"
                                    }
                                }
                            }
                        }
                    }
                }
            }
            
            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                
                TextArea {
                    readOnly: true
                    text: detailDialog.pack ? detailDialog.pack.description : ""
                    wrapMode: Text.WordWrap
                    color: Theme.textPrimary
                }
            }
            
            RowLayout {
                Layout.fillWidth: true
                spacing: Theme.spacingM
                
                Button {
                    text: qsTr("Open Website")
                    onClicked: {
                        if (detailDialog.pack && detailDialog.pack.url) {
                            Qt.openUrlExternally(detailDialog.pack.url)
                        }
                    }
                }
                
                Item { Layout.fillWidth: true }
                
                Button {
                    text: qsTr("Install Pack")
                    highlighted: true
                    onClicked: {
                        if (vm && detailDialog.pack) {
                            vm.installPack(detailDialog.pack.slug)
                            detailDialog.close()
                        }
                    }
                }
            }
        }
    }
}
