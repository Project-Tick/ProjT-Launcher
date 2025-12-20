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
    width: parent ? parent.width : 640
    height: parent ? parent.height : 480
    readonly property var vm: ProjT.newsVM

    // Theme binding for reactive updates
    property var themeVM: ProjT.themeVM
    property int _themeUpdateCount: 0

    color: {
        var _ = _themeUpdateCount;
        return themeVM ? themeVM.windowColor : ThemeColors.background;
    }

    Connections {
        target: themeVM
        function onThemeColorsChanged() {
            newsPage._themeUpdateCount++;
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingM
        spacing: Theme.spacingS

        PageHeader {
            Layout.fillWidth: true
            title: qsTr("News")
            subtitle: vm ? (vm.busy ? qsTr("Loading...") : qsTr("Keep up with the latest updates")) : ""
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingS

            ThemedButton {
                text: qsTr("Refresh")
                icon.name: "view-refresh"
                size: "small"
                enabled: vm ? !vm.busy : false
                onClicked: vm ? vm.refresh() : undefined
            }
            
            Item { Layout.fillWidth: true }
        }

        // Main Content Area
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: Theme.spacingM

            // Left Side: Latest News (Featured)
            Frame {
                Layout.fillWidth: true
                Layout.fillHeight: true
                
                background: Rectangle {
                    color: ThemeColors.surface
                    radius: Theme.radius
                    border.color: ThemeColors.border
                    border.width: 1
                }
                
                padding: Theme.spacingM

                ColumnLayout {
                    anchors.fill: parent
                    spacing: Theme.spacingS

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

                    ToolSeparator {
                        Layout.fillWidth: true
                        orientation: Qt.Horizontal
                    }

                    ScrollView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        clip: true
                        ScrollBar.vertical.policy: ScrollBar.AsNeeded

                        Text {
                            width: parent.availableWidth
                            text: vm ? vm.currentArticleHtml : ""
                            textFormat: Text.RichText
                            wrapMode: Text.WordWrap
                            color: ThemeColors.textSecondary
                            font.pointSize: 11
                            linkColor: ThemeColors.accent
                            onLinkActivated: Qt.openUrlExternally(link)
                        }
                    }
                    
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: Theme.spacingS
                        
                        Item { Layout.fillWidth: true }
                        
                        ThemedButton {
                            text: qsTr("Read in Browser")
                            icon.name: "internet-web-browser"
                            flatStyle: true
                            enabled: vm && vm.currentLink.length > 0
                            onClicked: vm ? vm.openCurrentLink() : undefined
                        }
                    }
                }
            }

            // Right Side: More News (List)
            Frame {
                Layout.preferredWidth: 300
                Layout.fillHeight: true
                
                background: Rectangle {
                    color: ThemeColors.backgroundAlt
                    radius: Theme.radius
                    border.color: ThemeColors.border
                    border.width: 1
                }

                padding: Theme.spacingS

                ColumnLayout {
                    anchors.fill: parent
                    spacing: Theme.spacingS
                    
                    Label {
                        text: qsTr("More Updates")
                        color: ThemeColors.text
                        font.bold: true
                        font.pointSize: 12
                        Layout.leftMargin: 4
                    }

                    ListView {
                        id: newsList
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        clip: true
                        spacing: 4
                        
                        model: vm ? vm.titles : []
                        currentIndex: vm ? vm.currentIndex : -1

                        delegate: ItemDelegate {
                            width: newsList.width
                            height: contentItem.implicitHeight + 16
                            
                            highlighted: ListView.isCurrentItem
                            
                            background: Rectangle {
                                color: highlighted ? ThemeColors.highlight : (hovered ? ThemeColors.surface : "transparent")
                                radius: Theme.radius
                                opacity: highlighted ? 0.1 : 1
                                border.color: highlighted ? ThemeColors.accent : "transparent"
                                border.width: 1
                            }

                            contentItem: ColumnLayout {
                                spacing: 2
                                Label {
                                    text: modelData
                                    color: highlighted ? ThemeColors.highlightedText : ThemeColors.text
                                    font.bold: highlighted
                                    elide: Text.ElideRight
                                    Layout.fillWidth: true
                                    wrapMode: Text.WordWrap
                                }
                            }

                            onClicked: {
                                if (vm) {
                                    vm.selectByIndex(index);
                                }
                            }
                        }
                        
                        ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }
                    }
                }
            }
        }
    }

    // Loading Overlay
    Rectangle {
        anchors.fill: parent
        color: ThemeColors.background
        opacity: vm && vm.busy ? 0.7 : 0
        visible: opacity > 0
        z: 100
        
        Behavior on opacity { NumberAnimation { duration: 150 } }
        
        ColumnLayout {
            anchors.centerIn: parent
            spacing: Theme.spacingM
            
            BusyIndicator {
                Layout.alignment: Qt.AlignHCenter
                running: vm ? vm.busy : false
            }
            
            Label {
                text: qsTr("Fetching news...")
                color: ThemeColors.text
                font.pointSize: 12
            }
        }
    }
}
