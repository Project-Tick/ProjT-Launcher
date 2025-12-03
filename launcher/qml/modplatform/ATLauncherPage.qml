// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme.js" as Theme

Page {
    id: atlPage
    title: qsTr("ATLauncher")
    
    property var vm: ProjT ? ProjT.atlVM : null
    property string searchQuery: ""
    property var searchResults: []
    property bool isSearching: false
    
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
                color: "#1e8e3e"
                
                Label {
                    anchors.centerIn: parent
                    text: "ATL"
                    color: "white"
                    font.bold: true
                    font.pointSize: 12
                }
            }
            
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 2
                
                Label {
                    text: qsTr("ATLauncher Packs")
                    color: Theme.textPrimary
                    font.bold: true
                    font.pointSize: Theme.fontSizeMedium
                }
                
                Label {
                    text: qsTr("Browse and install modpacks from ATLauncher")
                    color: Theme.textSecondary
                    font.pointSize: Theme.fontSizeSmall
                }
            }
        }
        
        // Search and filters
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingS
            
            TextField {
                id: searchField
                Layout.fillWidth: true
                placeholderText: qsTr("Search modpacks...")
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
            
            ComboBox {
                id: sortCombo
                Layout.preferredWidth: 150
                model: [
                    qsTr("Popular"),
                    qsTr("Updated"),
                    qsTr("Name")
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
        
        // Results
        Frame {
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            GridView {
                id: packGrid
                anchors.fill: parent
                clip: true
                cellWidth: 200
                cellHeight: 240
                model: searchResults.length > 0 ? searchResults : (vm ? vm.packs : [])
                
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
                        
                        // Pack icon
                        Rectangle {
                            Layout.alignment: Qt.AlignHCenter
                            Layout.preferredWidth: 96
                            Layout.preferredHeight: 96
                            radius: 8
                            color: Theme.backgroundAlt
                            
                            Image {
                                anchors.fill: parent
                                anchors.margins: 4
                                source: modelData.iconUrl || ""
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
                        
                        // Author
                        Label {
                            Layout.fillWidth: true
                            text: modelData.author || ""
                            color: Theme.textSecondary
                            font.pointSize: Theme.fontSizeSmall
                            horizontalAlignment: Text.AlignHCenter
                            elide: Text.ElideRight
                        }
                        
                        // Minecraft version
                        Label {
                            Layout.fillWidth: true
                            text: modelData.minecraftVersion || ""
                            color: Theme.accent
                            font.pointSize: Theme.fontSizeSmall
                            horizontalAlignment: Text.AlignHCenter
                        }
                        
                        Item { Layout.fillHeight: true }
                        
                        // Install button
                        Button {
                            Layout.fillWidth: true
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
            
            BusyIndicator {
                anchors.centerIn: parent
                running: isSearching || (vm ? vm.loading : false)
                visible: running
            }
            
            Label {
                anchors.centerIn: parent
                text: qsTr("No modpacks found")
                color: Theme.textSecondary
                visible: !isSearching && packGrid.count === 0
            }
        }
    }
    
    function performSearch() {
        searchQuery = searchField.text
        if (vm) {
            isSearching = true
            vm.search(searchQuery, sortCombo.currentIndex, versionCombo.currentText === "All Versions" ? "" : versionCombo.currentText)
        }
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
        width: 500
        height: 450
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
                        text: qsTr("by %1").arg(detailDialog.pack ? detailDialog.pack.author : "")
                        color: Theme.textSecondary
                    }
                    
                    Label {
                        text: qsTr("Minecraft %1").arg(detailDialog.pack ? detailDialog.pack.minecraftVersion : "")
                        color: Theme.accent
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
            
            Button {
                Layout.fillWidth: true
                text: qsTr("Install Pack")
                highlighted: true
                onClicked: {
                    if (vm && detailDialog.pack) {
                        vm.installPack(detailDialog.pack.id)
                        detailDialog.close()
                    }
                }
            }
        }
    }
}
