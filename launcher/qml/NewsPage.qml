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
import "components"

Rectangle {
    objectName: "news"
    color: "#1b1b1b"
    width: parent ? parent.width : 640
    height: parent ? parent.height : 480

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        PageHeader {
            Layout.fillWidth: true
            title: qsTr("News")
            subtitle: ProjT.newsVM ? ProjT.newsVM.currentTitle : ""
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 8
            Button {
                text: qsTr("Refresh")
                implicitHeight: 34
                implicitWidth: 90
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                enabled: ProjT.newsVM && !ProjT.newsVM.busy
                onClicked: ProjT.newsVM ? ProjT.newsVM.refresh() : undefined
            }
            Button {
                text: qsTr("Open in browser")
                implicitHeight: 34
                implicitWidth: 110
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                enabled: ProjT.newsVM && ProjT.newsVM.currentLink.length > 0
                onClicked: ProjT.newsVM ? ProjT.newsVM.openCurrentLink() : undefined
            }
            Rectangle { Layout.fillWidth: true; color: "transparent" }
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
                model: ProjT.newsVM ? ProjT.newsVM.titles : []
                delegate: Rectangle {
                    width: newsList.width
                    height: 48
                    color: ProjT.newsVM && index === ProjT.newsVM.currentIndex ? "#2c3440" : "#23262b"
                    border.color: "#323742"
                    radius: 4
                    Text {
                        anchors.fill: parent
                        anchors.margins: 8
                        text: modelData
                        color: "#e0e0e0"
                        wrapMode: Text.WordWrap
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if (ProjT.newsVM) {
                                ProjT.newsVM.selectByIndex(index)
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
                    ScrollView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Text {
                            width: parent ? parent.width : implicitWidth
                            wrapMode: Text.WordWrap
                            textFormat: Text.RichText
                            color: "#cfd8dc"
                            text: ProjT.newsVM ? ProjT.newsVM.currentArticleHtml : ""
                        }
                    }
                }
            }
        }
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#000000"
            opacity: ProjT.newsVM && ProjT.newsVM.busy ? 0.25 : 0
            visible: opacity > 0
            Behavior on opacity { NumberAnimation { duration: 150 } }
            BusyIndicator {
                anchors.centerIn: parent
                running: ProjT.newsVM ? ProjT.newsVM.busy : false
                visible: running
            }
        }
    }
}
