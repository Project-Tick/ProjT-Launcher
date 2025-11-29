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

Rectangle {
    objectName: "instances"
    color: "#1b1b1b"
    width: parent ? parent.width : 640
    height: parent ? parent.height : 480

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        Label {
            text: qsTr("Instances")
            color: "#eceff1"
            font.pixelSize: 18
            font.bold: true
        }

        ListView {
            id: instanceList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: ProjT.instancesVM ? ProjT.instancesVM.instanceNames : []
            delegate: Rectangle {
                width: instanceList.width
                height: 36
                color: (ProjT.instancesVM && modelData === ProjT.instancesVM.selectedInstanceId) ? "#2c3440" : "#23262b"
                border.color: "#323742"
                radius: 4

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    text: modelData
                    color: "#e0e0e0"
                    elide: Text.ElideRight
                    font.pointSize: 11
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (ProjT.instancesVM) {
                            ProjT.instancesVM.selectInstance(modelData)
                            console.log("Selected:", modelData)
                        }
                    }
                }
            }
        }
    }
}
