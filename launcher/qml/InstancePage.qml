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
    objectName: "instances"
    color: "#1b1b1b"
    width: parent ? parent.width : 640
    height: parent ? parent.height : 480
    property string selectedInstanceName: {
        if (!ProjT.instancesVM) {
            return ""
        }
        const ids = ProjT.instancesVM.instanceIds
        const names = ProjT.instancesVM.instanceNames
        const target = ProjT.instancesVM.selectedInstanceId
        const idx = ids.indexOf(target)
        if (idx >= 0 && idx < names.length) {
            return names[idx]
        }
        return ""
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        PageHeader {
            Layout.fillWidth: true
            title: qsTr("Instances")
            subtitle: qsTr("Manage and launch your instances")
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 8
            Button {
                text: qsTr("Launch")
                implicitHeight: 34
                implicitWidth: 90
                Layout.alignment: Qt.AlignVCenter
                enabled: ProjT.instancesVM && !ProjT.instancesVM.busy
                onClicked: ProjT.instancesVM ? ProjT.instancesVM.launchSelectedInstance() : undefined
            }
            Button {
                text: qsTr("Refresh")
                implicitHeight: 34
                implicitWidth: 90
                Layout.alignment: Qt.AlignVCenter
                enabled: ProjT.instancesVM && !ProjT.instancesVM.busy
                onClicked: ProjT.instancesVM ? ProjT.instancesVM.refreshInstances() : undefined
            }
            Button {
                text: qsTr("Delete")
                implicitHeight: 34
                implicitWidth: 90
                Layout.alignment: Qt.AlignVCenter
                enabled: ProjT.instancesVM && !ProjT.instancesVM.busy
                onClicked: {
                    if (ProjT.instancesVM) {
                        deleteDialog.open()
                    }
                }
            }
            Button {
                text: qsTr("Rename")
                implicitHeight: 34
                implicitWidth: 90
                Layout.alignment: Qt.AlignVCenter
                enabled: ProjT.instancesVM && !ProjT.instancesVM.busy
                onClicked: {
                    if (ProjT.instancesVM) {
                        renameDialog.open()
                    }
                }
            }
            Rectangle { Layout.fillWidth: true; color: "transparent" }
        }

        Frame {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Rectangle {
                anchors.fill: parent
                color: "#1b1b1b"
            }

            ListView {
                id: instanceList
                anchors.fill: parent
                clip: true
                model: ProjT.instancesVM ? ProjT.instancesVM.instanceNames.length : 0
                delegate: Rectangle {
                    width: instanceList.width
                    height: 42
                    color: (ProjT.instancesVM && ProjT.instancesVM.selectedInstanceId === ProjT.instancesVM.instanceIds[index]) ? "#2c3440" : "#23262b"
                    border.color: "#323742"
                    radius: 4

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 8
                        spacing: 8
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2
                            Label {
                                text: ProjT.instancesVM ? ProjT.instancesVM.instanceNames[index] : ""
                                color: "#e0e0e0"
                                font.bold: true
                                elide: Label.ElideRight
                            }
                            Label {
                                text: ProjT.instancesVM ? ProjT.instancesVM.instanceGroups[index] : ""
                                color: "#b0bec5"
                                font.pixelSize: 11
                                elide: Label.ElideRight
                            }
                        }
                        Button {
                            text: qsTr("Play")
                            implicitHeight: 34
                            implicitWidth: 90
                            onClicked: {
                                if (ProjT.instancesVM) {
                                    ProjT.instancesVM.selectInstanceByIndex(index)
                                    ProjT.instancesVM.launchSelectedInstance()
                                }
                            }
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            if (ProjT.instancesVM) {
                                ProjT.instancesVM.selectInstanceByIndex(index)
                            }
                        }
                        onDoubleClicked: {
                            if (ProjT.instancesVM) {
                                ProjT.instancesVM.selectInstanceByIndex(index)
                                ProjT.instancesVM.launchSelectedInstance()
                            }
                        }
                        acceptedButtons: Qt.LeftButton | Qt.RightButton
                        onPressed: {
                            if (mouse.button === Qt.RightButton) {
                                contextMenu.popup()
                            }
                        }
                    }

                    Menu {
                        id: contextMenu
                        MenuItem { text: qsTr("Launch"); onTriggered: ProjT.instancesVM.launchSelectedInstance() }
                        MenuItem { text: qsTr("Edit (settings)"); onTriggered: ProjT.instancesVM.selectInstanceByIndex(index) }
                        MenuItem { text: qsTr("Duplicate"); onTriggered: ProjT.instancesVM.duplicateSelectedInstance(ProjT.instancesVM.instanceNames[index] + qsTr(" Copy")) }
                        MenuItem { text: qsTr("Rename"); onTriggered: renameDialog.open() }
                        MenuItem { text: qsTr("Delete"); onTriggered: deleteDialog.open() }
                    }
                }
            }
            Rectangle {
                anchors.fill: parent
                color: "#000000"
                opacity: ProjT.instancesVM && ProjT.instancesVM.busy ? 0.25 : 0
                visible: opacity > 0
                Behavior on opacity { NumberAnimation { duration: 150 } }
                BusyIndicator {
                    anchors.centerIn: parent
                    running: ProjT.instancesVM ? ProjT.instancesVM.busy : false
                    visible: running
                }
            }
        }
    }

    Dialog {
        id: renameDialog
        modal: true
        focus: true
        standardButtons: Dialog.Ok | Dialog.Cancel
        title: qsTr("Rename Instance")
        property alias newName: nameField.text
        width: 360
        contentItem: ColumnLayout {
            anchors.fill: parent
            anchors.margins: 12
            spacing: 8
            Label { text: qsTr("New name"); color: "#e0e0e0" }
            TextField { id: nameField; Layout.fillWidth: true }
        }
        onAccepted: {
            if (ProjT.instancesVM) {
                ProjT.instancesVM.renameSelectedInstance(newName)
            }
        }
    }

    Dialog {
        id: deleteDialog
        modal: true
        focus: true
        standardButtons: Dialog.Yes | Dialog.No
        title: qsTr("Confirm Deletion")
        width: 360
        contentItem: ColumnLayout {
            anchors.fill: parent
            anchors.margins: 12
            spacing: 8
            Label {
                wrapMode: Text.WordWrap
                text: selectedInstanceName.length
                      ? qsTr("You are about to delete \"%1\". This action may be permanent. Continue?").arg(selectedInstanceName)
                      : qsTr("You are about to delete the selected instance. Continue?")
                color: "#e0e0e0"
            }
        }
        onAccepted: {
            if (ProjT.instancesVM) {
                ProjT.instancesVM.deleteSelectedInstance()
            }
        }
    }
}
