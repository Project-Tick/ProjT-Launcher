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

Rectangle {
    id: technicPage
    color: ThemeColors.background
    
    property var vm: typeof ProjT !== "undefined" && ProjT ? ProjT.technicVM : null
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingM
        spacing: Theme.spacingS
        
        // Search row
        TextField {
            id: searchField
            Layout.fillWidth: true
            placeholderText: qsTr("Search and filter...")
            onAccepted: if (vm) vm.search(text)
            onTextChanged: {
                if (vm && text.length > 2) vm.search(text)
            }
        }
        
        // Main content
        SplitView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            orientation: Qt.Horizontal
            
            // Pack list
            ListView {
                id: packList
                SplitView.fillWidth: true
                SplitView.fillHeight: true
                SplitView.preferredWidth: 350
                clip: true
                model: vm ? vm.packsModel : null
                currentIndex: vm ? vm.selectedPackIndex : -1
                
                ScrollBar.vertical: ScrollBar {}
                
                delegate: ItemDelegate {
                    width: packList.width
                    height: 58
                    highlighted: ListView.isCurrentItem
                    
                    background: Rectangle {
                        color: highlighted ? ThemeColors.primary : (index % 2 === 0 ? "transparent" : ThemeColors.backgroundAlt)
                        opacity: highlighted ? 0.2 : 0.3
                    }
                    
                    onClicked: {
                        if (vm) vm.selectPack(index)
                    }
                    
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 4
                        spacing: 8
                        
                        Image {
                            Layout.preferredWidth: 48
                            Layout.preferredHeight: 48
                            source: model.iconUrl || ""
                            fillMode: Image.PreserveAspectFit
                            
                            Rectangle {
                                anchors.fill: parent
                                visible: parent.status !== Image.Ready
                                color: ThemeColors.surface
                                
                                Label {
                                    anchors.centerIn: parent
                                    text: "T"
                                    color: ThemeColors.error
                                    font.bold: true
                                    font.pointSize: 16
                                }
                            }
                        }
                        
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2
                            
                            Label {
                                text: model.name || ""
                                color: ThemeColors.text
                                font.bold: true
                                elide: Text.ElideRight
                                Layout.fillWidth: true
                            }
                            
                            Label {
                                text: model.description ? model.description.substring(0, 100) : ""
                                color: ThemeColors.textSecondary
                                font.pointSize: 9
                                elide: Text.ElideRight
                                Layout.fillWidth: true
                            }
                        }
                    }
                }
                
                BusyIndicator {
                    anchors.centerIn: parent
                    running: vm ? vm.isLoading : false
                    visible: running && packList.count === 0
                }
                
                Label {
                    anchors.centerIn: parent
                    visible: packList.count === 0 && !(vm && vm.isLoading)
                    text: qsTr("Search for Technic modpacks")
                    color: ThemeColors.textSecondary
                    horizontalAlignment: Text.AlignHCenter
                }
            }
            
            // Pack description
            ScrollView {
                id: descriptionView
                SplitView.fillWidth: true
                SplitView.fillHeight: true
                SplitView.preferredWidth: 400
                clip: true
                
                TextArea {
                    id: packDescription
                    readOnly: true
                    textFormat: TextArea.RichText
                    wrapMode: TextArea.Wrap
                    color: ThemeColors.text
                    
                    text: {
                        if (!vm || !vm.selectedPack || !vm.selectedPack.name) {
                            return "<i>" + qsTr("Select a modpack to view details") + "</i>"
                        }
                        
                        var html = ""
                        var pack = vm.selectedPack
                        
                        // Name with link
                        if (pack.websiteUrl) {
                            html += '<a href="' + pack.websiteUrl + '">' + pack.name + '</a>'
                        } else {
                            html += "<b>" + pack.name + "</b>"
                        }
                        
                        // Author
                        if (pack.author) {
                            html += "<br>" + qsTr(" by ") + pack.author
                        }
                        
                        html += "<br><br>"
                        
                        // Description
                        if (pack.description) {
                            html += pack.description
                        }
                        
                        return html
                    }
                    
                    onLinkActivated: function(link) {
                        Qt.openUrlExternally(link)
                    }
                }
            }
        }
        
        // Bottom bar
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingS
            
            Item { Layout.fillWidth: true }
            
            Label {
                text: qsTr("Version selected:")
                color: ThemeColors.text
            }
            
            ComboBox {
                id: versionCombo
                Layout.preferredWidth: 200
                model: vm ? vm.selectedPackVersions : []
                currentIndex: vm ? vm.selectedVersionIndex : -1
                enabled: count > 0
                onCurrentIndexChanged: {
                    if (vm && currentIndex >= 0 && currentIndex !== vm.selectedVersionIndex) {
                        vm.selectVersion(currentIndex)
                    }
                }
            }
            
            Button {
                text: qsTr("Install")
                enabled: vm && vm.selectedPackVersions && vm.selectedPackVersions.length > 0
                onClicked: {
                    if (vm) {
                        vm.installSelected("", "")
                    }
                }
            }
        }
    }
    
    // Loading metadata indicator
    BusyIndicator {
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: Theme.spacingM
        running: vm ? vm.isLoadingMetadata : false
        visible: running
    }
}
