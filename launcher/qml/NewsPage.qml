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
        var _ = _themeUpdateCount
        return themeVM ? themeVM.windowColor : ThemeColors.background
    }
    
    Connections {
        target: themeVM
        function onThemeColorsChanged() {
            newsPage._themeUpdateCount++
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingM
        spacing: Theme.spacingS
        Component.onCompleted: {
            if (vm) {
                if (vm.titles.length === 0) {
                    vm.refresh()
                } else if (vm.currentIndex < 0 && vm.titles.length > 0) {
                    vm.setCurrentIndex(0)
                }
            }
        }

        PageHeader {
            Layout.fillWidth: true
            title: qsTr("News")
            subtitle: vm ? vm.currentTitle : ""
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 8
            ThemedButton {
                text: qsTr("Refresh")
                size: "small"
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                enabled: vm ? !vm.busy : false
                onClicked: {
                    if (vm) {
                        vm.refresh()
                    }
                }
            }
            ThemedButton {
                text: qsTr("Open in browser")
                size: "small"
                primary: true
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                enabled: vm ? (vm.currentLink.length > 0) : false
                onClicked: {
                    if (vm) {
                        vm.openCurrentLink()
                    }
                }
            }
            Item { Layout.fillWidth: true }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 12

            ListView {
                id: newsList
                Layout.preferredWidth: 220
                Layout.fillHeight: true
                clip: true
                model: vm ? vm.titles : []
                currentIndex: vm ? vm.currentIndex : -1
                delegate: Rectangle {
                    width: newsList.width
                    height: 48
                    color: vm && index === vm.currentIndex ? Qt.rgba(ThemeColors.highlight.r, ThemeColors.highlight.g, ThemeColors.highlight.b, 0.2) : ThemeColors.surface
                    border.color: vm && index === vm.currentIndex ? ThemeColors.highlight : ThemeColors.border
                    border.width: vm && index === vm.currentIndex ? 2 : 1
                    radius: Theme.radius
                    
                    Behavior on color { ColorAnimation { duration: 100 } }
                    Behavior on border.color { ColorAnimation { duration: 100 } }
                    
                    Text {
                        anchors.fill: parent
                        anchors.margins: 8
                        text: modelData
                        color: ThemeColors.text
                        wrapMode: Text.WordWrap
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if (vm) {
                                vm.selectByIndex(index)
                            }
                        }
                    }
                }
            }

            Frame {
                Layout.fillWidth: true
                Layout.fillHeight: true
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 8
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8
                        Text {
                            Layout.fillWidth: true
                            text: vm ? vm.currentTitle : ""
                            color: ThemeColors.text
                            font.pixelSize: 16
                            wrapMode: Text.WordWrap
                        }
                        Text {
                            text: vm && vm.lastUpdated && vm.lastUpdated.toString().length > 0
                                  ? qsTr("Updated: %1").arg(vm.lastUpdated.toString())
                                  : ""
                            color: ThemeColors.textSecondary
                            font.pixelSize: 12
                            horizontalAlignment: Text.AlignRight
                        }
                    }
                    ScrollView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Text {
                            width: parent ? parent.width : implicitWidth
                            wrapMode: Text.WordWrap
                            textFormat: Text.RichText
                            color: ThemeColors.textSecondary
                            text: vm ? vm.currentArticleHtml : ""
                        }
                    }
                }
            }
        }
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: ThemeColors.backgroundAlt
            opacity: vm ? (vm.busy ? 0.25 : 0) : 0
            visible: opacity > 0
            Behavior on opacity { NumberAnimation { duration: 150 } }
            BusyIndicator {
                anchors.centerIn: parent
                running: vm ? vm.busy : false
                visible: running
            }
        }
    }
}
