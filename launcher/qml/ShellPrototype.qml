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
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

Rectangle {
    id: root
    width: 520
    height: 420
    color: "#202125"
    radius: 6

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 12

        Rectangle {
            Layout.fillWidth: true
            color: "#2b2d31"
            radius: 4
            border.color: "#404249"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 2

                Text {
                    text: launcherViewModel ? launcherViewModel.displayName : qsTr("Launcher")
                    font.pointSize: 18
                    font.bold: true
                    color: "white"
                }
                Text {
                    text: launcherViewModel ? launcherViewModel.versionString : ""
                    color: "#b0bec5"
                }
                Text {
                    text: launcherViewModel && launcherViewModel.busy ? qsTr("Status: Busy") : qsTr("Status: Idle")
                    color: launcherViewModel && launcherViewModel.busy ? "#fdd835" : "#81c784"
                    font.pointSize: 11
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Button {
                text: qsTr("Reload News")
                Layout.preferredWidth: 140
                onClicked: newsViewModel ? newsViewModel.requestRefresh() : null
            }

            Button {
                text: qsTr("Log Selection")
                Layout.preferredWidth: 160
                enabled: instanceListViewModel
                onClicked: {
                    if (instanceListViewModel) {
                        console.log("Selected instance:", instanceListViewModel.selectedInstanceId)
                    }
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#2b2d31"
            radius: 4
            border.color: "#404249"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 6

                Text {
                    text: instanceListViewModel ? qsTr("Instances (%1)").arg(instanceListViewModel.totalCount) : qsTr("Instances")
                    color: "white"
                    font.bold: true
                }

                ListView {
                    id: instanceList
                    Layout.fillWidth: true
                    Layout.preferredHeight: 140
                    clip: true
                    model: instanceListViewModel ? instanceListViewModel.instanceNames : []
                    delegate: Rectangle {
                        width: instanceList.width
                        height: 28
                        color: (modelData === (instanceListViewModel ? instanceListViewModel.selectedInstanceId : "")) ? "#39424e" : "#00000000"
                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: parent.left
                            anchors.leftMargin: 8
                            text: modelData
                            color: "#eceff1"
                        }
                    }
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 150
            color: "#2b2d31"
            radius: 4
            border.color: "#404249"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 6

                Text {
                    text: newsViewModel ? (newsViewModel.currentTitle || qsTr("Latest News")) : qsTr("Latest News")
                    font.bold: true
                    color: "white"
                    wrapMode: Text.WordWrap
                }

                Flickable {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    contentWidth: width
                    contentHeight: newsText.paintedHeight
                    clip: true

                    Text {
                        id: newsText
                        width: parent.width
                        wrapMode: Text.WordWrap
                        textFormat: Text.RichText
                        color: "#cfd8dc"
                        text: newsViewModel ? newsViewModel.currentContent : ""
                    }
                }
            }
        }
    }
}
