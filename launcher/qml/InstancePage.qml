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
import "Theme.js" as Theme

Rectangle {
    objectName: "instances"
    color: Theme.background
    width: parent ? parent.width : 640
    height: parent ? parent.height : 480
    readonly property var vm: ProjT.instancesVM
    property string selectedInstanceName: {
        if (!vm) {
            return ""
        }
        const ids = vm.instanceIds
        const names = vm.instanceNames
        const target = vm.selectedInstanceId
        const idx = ids.indexOf(target)
        if (idx >= 0 && idx < names.length) {
            return names[idx]
        }
        return ""
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingM
        spacing: Theme.spacingS

        PageHeader {
            Layout.fillWidth: true
            title: qsTr("Instances")
            subtitle: qsTr("Manage and launch your instances")
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 8
            RowLayout {
                spacing: 8
                Layout.fillWidth: true

                Button {
                    text: qsTr("Launch")
                    implicitHeight: 34
                    implicitWidth: 90
                    Layout.alignment: Qt.AlignVCenter
                    enabled: vm && !vm.busy && vm.canLaunchSelected
                    onClicked: vm ? vm.launchSelectedInstance() : undefined
                }
                Button {
                    text: qsTr("Refresh")
                    implicitHeight: 34
                    implicitWidth: 90
                    Layout.alignment: Qt.AlignVCenter
                    enabled: vm && !vm.busy
                    onClicked: vm ? vm.refreshInstances() : undefined
                }
                Button {
                    text: qsTr("Delete")
                    implicitHeight: 34
                    implicitWidth: 90
                    Layout.alignment: Qt.AlignVCenter
                    enabled: vm && !vm.busy && vm.canDeleteSelected
                    onClicked: {
                        if (vm) {
                            deleteDialog.open()
                        }
                    }
                }
                Button {
                    text: qsTr("Rename")
                    implicitHeight: 34
                    implicitWidth: 90
                    Layout.alignment: Qt.AlignVCenter
                    enabled: vm && !vm.busy && vm.hasSelection
                    onClicked: {
                        if (vm) {
                            renameDialog.open()
                        }
                    }
                }
                Rectangle { Layout.fillWidth: true; color: "transparent" }
            }
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
                spacing: 6
                model: vm ? vm.instanceIds.length : 0
                currentIndex: vm ? vm.instanceIds.indexOf(vm.selectedInstanceId) : -1
                onCurrentIndexChanged: {
                    if (vm && currentIndex >= 0 && currentIndex < vm.instanceIds.length) {
                        const id = vm.instanceIds[currentIndex]
                        if (id !== vm.selectedInstanceId) {
                            vm.selectInstanceByIndex(currentIndex)
                        }
                    }
                }
                delegate: Rectangle {
                    width: instanceList.width
                    height: 60
                    radius: Theme.radius
                    color: ListView.isCurrentItem ? "#2c3440" : (ma.containsMouse ? "#262a31" : Theme.surface)
                    border.color: ListView.isCurrentItem ? Theme.accent : "#323742"
                    border.width: 1

                    MouseArea {
                        id: ma
                        anchors.fill: parent
                        hoverEnabled: true
                        acceptedButtons: Qt.LeftButton | Qt.RightButton
                        onClicked: {
                            if (vm) {
                                vm.selectInstanceByIndex(index)
                            }
                        }
                        onPressed: function(mouse) {
                            if (mouse.button === Qt.RightButton) {
                                if (vm) {
                                    vm.selectInstanceByIndex(index)
                                }
                                contextMenu.popup()
                            }
                        }
                        onDoubleClicked: {
                            if (vm) {
                                vm.selectInstanceByIndex(index)
                                vm.launchSelectedInstance()
                            }
                        }
                    }

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: Theme.spacingM
                        spacing: Theme.spacingS

                        Rectangle {
                            width: 40
                            height: 40
                            radius: Theme.radius
                            color: Theme.surfaceVariant
                            border.color: "#2e333d"
                            Image {
                                anchors.centerIn: parent
                                width: 32
                                height: 32
                                source: vm && vm.instanceIconPaths.length > index ? vm.instanceIconPaths[index] : ""
                                fillMode: Image.PreserveAspectFit
                                visible: source !== ""
                            }
                        }

                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2
                            Label {
                                text: vm ? vm.instanceNames[index] : ""
                                color: Theme.textPrimary
                                font.bold: true
                                elide: Label.ElideRight
                            }
                            Label {
                                text: vm ? vm.instanceGroups[index] : ""
                                color: Theme.textSecondary
                                font.pixelSize: 11
                                elide: Label.ElideRight
                            }
                        }

                        Button {
                            text: qsTr("Play")
                            implicitHeight: 32
                            implicitWidth: 80
                            Layout.alignment: Qt.AlignVCenter
                            enabled: vm && vm.canLaunchSelected
                            onClicked: {
                                if (vm) {
                                    vm.selectInstanceByIndex(index)
                                    vm.launchSelectedInstance()
                                }
                            }
                        }
                    }

                    Menu {
                        id: contextMenu
                        MenuItem { text: qsTr("Launch"); enabled: vm && vm.canLaunchSelected; onTriggered: vm ? vm.launchSelectedInstance() : undefined }
                        MenuItem { text: qsTr("Edit (settings)"); enabled: vm && vm.hasSelection; onTriggered: vm ? vm.selectInstanceByIndex(index) : undefined }
                        MenuItem { text: qsTr("Duplicate"); enabled: vm && vm.hasSelection; onTriggered: vm ? vm.duplicateSelectedInstance(vm.instanceNames[index] + qsTr(" Copy")) : undefined }
                        MenuItem { text: qsTr("Rename"); enabled: vm && vm.hasSelection; onTriggered: renameDialog.open() }
                        MenuItem { text: qsTr("Delete"); enabled: vm && vm.canDeleteSelected; onTriggered: deleteDialog.open() }
                    }
                }

                Connections {
                    target: vm
                    function onSelectedInstanceIdChanged() {
                        if (!vm) {
                            return
                        }
                        instanceList.currentIndex = vm.instanceIds.indexOf(vm.selectedInstanceId)
                    }
                    function onInstanceListChanged() {
                        if (!vm) {
                            return
                        }
                        instanceList.currentIndex = vm.instanceIds.indexOf(vm.selectedInstanceId)
                    }
                }
            }
            Rectangle {
                anchors.fill: parent
                color: "#000000"
                opacity: vm && vm.busy ? 0.25 : 0
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
            if (vm) {
                vm.renameSelectedInstance(newName)
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
            if (vm) {
                vm.deleteSelectedInstance()
            }
        }
    }
}
