// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import ProjTLauncher 1.0
import "components"
import "Theme.js" as Theme

Rectangle {
    id: newsPage
    objectName: "news"
    color: ThemeColors.background
    width: parent ? parent.width : 640
    height: parent ? parent.height : 480

    readonly property var vm: ProjT.newsVM

    Component.onCompleted: {
        if (vm) {
            vm.refresh();
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingS
        spacing: Theme.spacingS

        // === Header ===
        PageHeader {
            Layout.fillWidth: true
            title: qsTr("News")
            subtitle: vm ? (vm.busy ? qsTr("Loading...") : qsTr("Keep up with the latest updates")) : ""
        }

        // === Main Layout ===
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: Theme.spacingS

            // --- Left Section: Article Viewer ---
            Frame {
                Layout.fillWidth: true
                Layout.fillHeight: true
                padding: Theme.spacingM
                
                background: Rectangle {
                    color: ThemeColors.surface
                    radius: Theme.radius
                    border.color: ThemeColors.border
                    border.width: 1
                }

                ColumnLayout {
                    anchors.fill: parent
                    spacing: Theme.spacingS

                    // Article Title & Meta
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: Theme.spacingS
                        
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2
                            
                            Label {
                                text: vm ? vm.currentTitle : ""
                                color: ThemeColors.text
                                font.pointSize: 16
                                font.bold: true
                                Layout.fillWidth: true
                                wrapMode: Text.WordWrap
                            }

                            Label {
                                text: vm && vm.lastUpdated ? qsTr("Posted on %1").arg(vm.lastUpdated.toString()) : ""
                                color: ThemeColors.textSecondary
                                font.pointSize: 10
                            }
                        }

                        // Top actions
                        ThemedButton {
                            icon.name: "view-refresh"
                            size: "small"
                            flatStyle: true
                            enabled: vm ? !vm.busy : false
                            onClicked: vm ? vm.refresh() : undefined
                            ToolTip.text: qsTr("Refresh Feed")
                            ToolTip.visible: hovered
                        }
                    }

                    ToolSeparator {
                        Layout.fillWidth: true
                        orientation: Qt.Horizontal
                    }

                    // Content Scroll Area
                    ScrollView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        clip: true
                        
                        TextArea {
                            readOnly: true
                            selectByMouse: true
                            text: vm ? vm.currentArticleHtml : ""
                            textFormat: Text.RichText
                            wrapMode: Text.WordWrap
                            color: ThemeColors.text
                            font.pointSize: 11
                            background: null
                            padding: 0
                            
                            onLinkActivated: (link) => Qt.openUrlExternally(link)
                        }
                    }
                    
                    // Footer Actions
                    RowLayout {
                        Layout.fillWidth: true
                        
                        Item { Layout.fillWidth: true }
                        
                        ThemedButton {
                            text: qsTr("Open in Browser")
                            icon.name: "internet-web-browser"
                            size: "small"
                            outline: true
                            enabled: vm && vm.currentLink.length > 0
                            onClicked: vm ? vm.openCurrentLink() : undefined
                        }
                    }
                }
            }

            // --- Right Section: News List ---
            Frame {
                Layout.preferredWidth: 280
                Layout.fillHeight: true
                padding: 0 // We'll use list margins
                
                background: Rectangle {
                    color: ThemeColors.backgroundAlt
                    radius: Theme.radius
                    border.color: ThemeColors.border
                    border.width: 1
                }

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 0
                    
                    Label {
                        text: qsTr("ALL UPDATES")
                        color: ThemeColors.textSecondary
                        font.bold: true
                        font.pointSize: 9
                        Layout.margins: Theme.spacingS
                    }

                    ListView {
                        id: newsList
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        clip: true
                        spacing: 1
                        
                        model: vm ? vm.titles : []
                        currentIndex: vm ? vm.currentIndex : -1

                        delegate: ItemDelegate {
                            width: newsList.width
                            height: 54
                            
                            highlighted: ListView.isCurrentItem
                            
                            background: Rectangle {
                                color: highlighted ? ThemeColors.highlight : (hovered ? ThemeColors.surface : "transparent")
                                opacity: highlighted ? 0.15 : 1
                                
                                Rectangle {
                                    anchors.left: parent.left
                                    anchors.verticalCenter: parent.verticalCenter
                                    width: 3
                                    height: parent.height * 0.6
                                    color: ThemeColors.accent
                                    visible: highlighted
                                }
                            }

                            contentItem: ColumnLayout {
                                spacing: 2
                                anchors.leftMargin: highlighted ? 12 : 8
                                
                                Label {
                                    text: modelData
                                    color: highlighted ? ThemeColors.highlightedText : ThemeColors.text
                                    font.bold: highlighted
                                    elide: Text.ElideRight
                                    Layout.fillWidth: true
                                }
                                
                                Label {
                                    text: qsTr("Click to read")
                                    color: ThemeColors.textSecondary
                                    font.pointSize: 9
                                    visible: !highlighted && hovered
                                }
                            }

                            onClicked: {
                                if (vm) {
                                    vm.selectByIndex(index);
                                }
                            }
                        }
                        
                        ScrollBar.vertical: ScrollBar { 
                            active: true
                        }
                    }
                }
            }
        }
    }

    // === Loading State ===
    Rectangle {
        anchors.fill: parent
        color: ThemeColors.background
        opacity: vm && vm.busy ? 0.6 : 0
        visible: opacity > 0
        z: 100
        
        Behavior on opacity { NumberAnimation { duration: 200 } }
        
        ColumnLayout {
            anchors.centerIn: parent
            spacing: Theme.spacingM
            
            BusyIndicator {
                Layout.alignment: Qt.AlignHCenter
                running: vm ? vm.busy : false
            }
            
            Label {
                text: qsTr("Fetching news content...")
                color: ThemeColors.text
                font.pointSize: 11
            }
        }
    }
}
