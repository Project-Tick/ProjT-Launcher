// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme.js" as Theme

Page {
    id: ftbPage
    title: qsTr("FTB")
    
    property var vm: ProjT ? ProjT.ftbVM : null
    property string searchQuery: ""
    property var searchResults: []
    property bool isSearching: false
    property string currentTab: "featured"
    
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
                color: "#1a1a2e"
                
                Label {
                    anchors.centerIn: parent
                    text: "FTB"
                    color: "#e94560"
                    font.bold: true
                    font.pointSize: 12
                }
            }
            
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 2
                
                Label {
                    text: qsTr("Feed The Beast")
                    color: Theme.textPrimary
                    font.bold: true
                    font.pointSize: Theme.fontSizeMedium
                }
                
                Label {
                    text: qsTr("Browse official FTB modpacks")
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
                text: qsTr("Featured")
                onClicked: currentTab = "featured"
            }
            
            TabButton {
                text: qsTr("All Packs")
                onClicked: currentTab = "all"
            }
            
            TabButton {
                text: qsTr("Search")
                onClicked: currentTab = "search"
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
                placeholderText: qsTr("Search FTB modpacks...")
                onAccepted: performSearch()
            }
            
            Button {
                text: qsTr("Search")
                highlighted: true
                onClicked: performSearch()
            }
        }
        
        // Filter row
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingM
            visible: currentTab !== "featured"
            
            ComboBox {
                id: sortCombo
                Layout.preferredWidth: 150
                model: [
                    qsTr("Installs"),
                    qsTr("Updated"),
                    qsTr("Name"),
                    qsTr("Plays")
                ]
            }
            
            ComboBox {
                id: versionCombo
                Layout.preferredWidth: 150
                model: ["All Versions"].concat(vm ? vm.minecraftVersions : [])
            }
            
            Item { Layout.fillWidth: true }
            
            Button {
                text: qsTr("Refresh")
                icon.name: "view-refresh"
                onClicked: {
                    if (vm) vm.refresh()
                }
            }
        }
        
        // Content
        Frame {
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            StackLayout {
                anchors.fill: parent
                currentIndex: currentTab === "featured" ? 0 : (currentTab === "all" ? 1 : 2)
                
                // Featured packs
                ListView {
                    id: featuredList
                    clip: true
                    model: vm ? vm.featuredPacks : []
                    spacing: 8
                    
                    delegate: Rectangle {
                        width: featuredList.width
                        height: 100
                        radius: 8
                        color: mouseArea.containsMouse ? Theme.backgroundAlt : "transparent"
                        border.color: Theme.divider
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
                        
                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: Theme.spacingM
                            spacing: Theme.spacingM
                            
                            Rectangle {
                                Layout.preferredWidth: 72
                                Layout.preferredHeight: 72
                                radius: 8
                                color: Theme.backgroundAlt
                                
                                Image {
                                    anchors.fill: parent
                                    anchors.margins: 4
                                    source: modelData.iconUrl || ""
                                    fillMode: Image.PreserveAspectFit
                                }
                            }
                            
                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 4
                                
                                Label {
                                    text: modelData.name || qsTr("Unknown")
                                    color: Theme.textPrimary
                                    font.bold: true
                                }
                                
                                Label {
                                    Layout.fillWidth: true
                                    text: modelData.synopsis || ""
                                    color: Theme.textSecondary
                                    font.pointSize: Theme.fontSizeSmall
                                    elide: Text.ElideRight
                                    maximumLineCount: 2
                                    wrapMode: Text.WordWrap
                                }
                                
                                RowLayout {
                                    spacing: Theme.spacingM
                                    
                                    Label {
                                        text: qsTr("MC %1").arg(modelData.minecraftVersion || "")
                                        color: Theme.accent
                                        font.pointSize: Theme.fontSizeSmall
                                    }
                                    
                                    Label {
                                        text: qsTr("%1 installs").arg(formatNumber(modelData.installs || 0))
                                        color: Theme.textSecondary
                                        font.pointSize: Theme.fontSizeSmall
                                    }
                                }
                            }
                            
                            Button {
                                text: qsTr("Install")
                                highlighted: true
                                onClicked: {
                                    if (vm) vm.installPack(modelData.id)
                                }
                            }
                        }
                    }
                    
                    ScrollBar.vertical: ScrollBar {}
                }
                
                // All packs
                GridView {
                    id: allPacksGrid
                    clip: true
                    cellWidth: 180
                    cellHeight: 220
                    model: vm ? vm.allPacks : []
                    
                    delegate: packDelegate
                    ScrollBar.vertical: ScrollBar {}
                }
                
                // Search results
                GridView {
                    id: searchGrid
                    clip: true
                    cellWidth: 180
                    cellHeight: 220
                    model: searchResults
                    
                    delegate: packDelegate
                    ScrollBar.vertical: ScrollBar {}
                }
            }
            
            BusyIndicator {
                anchors.centerIn: parent
                running: isSearching || (vm ? vm.loading : false)
                visible: running
            }
        }
    }
    
    Component {
        id: packDelegate
        
        Rectangle {
            width: 172
            height: 212
            radius: 8
            color: packMouseArea.containsMouse ? Theme.backgroundAlt : "transparent"
            border.color: packMouseArea.containsMouse ? Theme.accent : Theme.divider
            border.width: 1
            
            MouseArea {
                id: packMouseArea
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
                
                Rectangle {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.preferredWidth: 80
                    Layout.preferredHeight: 80
                    radius: 8
                    color: Theme.backgroundAlt
                    
                    Image {
                        anchors.fill: parent
                        anchors.margins: 4
                        source: modelData.iconUrl || ""
                        fillMode: Image.PreserveAspectFit
                    }
                }
                
                Label {
                    Layout.fillWidth: true
                    text: modelData.name || ""
                    color: Theme.textPrimary
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    elide: Text.ElideRight
                    maximumLineCount: 2
                    wrapMode: Text.WordWrap
                }
                
                Label {
                    Layout.fillWidth: true
                    text: qsTr("MC %1").arg(modelData.minecraftVersion || "")
                    color: Theme.accent
                    font.pointSize: Theme.fontSizeSmall
                    horizontalAlignment: Text.AlignHCenter
                }
                
                Item { Layout.fillHeight: true }
                
                Button {
                    Layout.fillWidth: true
                    text: qsTr("Install")
                    onClicked: {
                        if (vm) vm.installPack(modelData.id)
                    }
                }
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
            
            RowLayout {
                Layout.fillWidth: true
                spacing: Theme.spacingM
                
                Rectangle {
                    Layout.preferredWidth: 96
                    Layout.preferredHeight: 96
                    radius: 8
                    color: Theme.backgroundAlt
                    
                    Image {
                        anchors.fill: parent
                        anchors.margins: 4
                        source: detailDialog.pack ? detailDialog.pack.iconUrl : ""
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
                        text: qsTr("by %1").arg(detailDialog.pack ? (detailDialog.pack.authors || []).join(", ") : "")
                        color: Theme.textSecondary
                    }
                    
                    RowLayout {
                        spacing: Theme.spacingM
                        
                        Label {
                            text: qsTr("MC %1").arg(detailDialog.pack ? detailDialog.pack.minecraftVersion : "")
                            color: Theme.accent
                        }
                        
                        Label {
                            text: qsTr("%1 installs").arg(formatNumber(detailDialog.pack ? detailDialog.pack.installs : 0))
                            color: Theme.textSecondary
                        }
                    }
                }
            }
            
            // Version selector
            RowLayout {
                Layout.fillWidth: true
                
                Label {
                    text: qsTr("Version:")
                }
                
                ComboBox {
                    id: versionSelect
                    Layout.fillWidth: true
                    model: detailDialog.pack ? detailDialog.pack.versions : []
                    textRole: "name"
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
            
            Button {
                Layout.fillWidth: true
                text: qsTr("Install Pack")
                highlighted: true
                onClicked: {
                    if (vm && detailDialog.pack) {
                        var version = versionSelect.currentIndex >= 0 ? detailDialog.pack.versions[versionSelect.currentIndex] : null
                        vm.installPack(detailDialog.pack.id, version ? version.id : null)
                        detailDialog.close()
                    }
                }
            }
        }
    }
}
