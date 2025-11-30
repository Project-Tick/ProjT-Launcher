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
    objectName: "logs"
    color: Theme.background
    width: parent ? parent.width : 640
    height: parent ? parent.height : 480
    readonly property var vm: ProjT.logsVM

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingM
        spacing: Theme.spacingS
        Component.onCompleted: {
            if (vm) {
                vm.loadLogs(vm.selectedLog)
            }
        }

        PageHeader {
            Layout.fillWidth: true
            title: qsTr("Logs")
            subtitle: qsTr("View launcher and instance logs")
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            ComboBox {
                id: logSelector
                Layout.fillWidth: true
                model: vm ? vm.logList : []
                textRole: ""
                onActivated: {
                    if (vm && currentIndex >= 0 && currentIndex < model.length) {
                        vm.selectedLog = model[currentIndex]
                    }
                }
                Component.onCompleted: {
                    if (vm && vm.logList.length > 0 && vm.selectedLog === "") {
                        vm.selectedLog = vm.logList[0]
                    }
                }
                delegate: ItemDelegate {
                    width: logSelector.width
                    text: modelData
                    highlighted: logSelector.currentIndex === index
                    onClicked: {
                        logSelector.currentIndex = index
                        logSelector.activated(index)
                    }
                }
            }

            Button {
                text: qsTr("Refresh")
                implicitHeight: 34
                implicitWidth: 90
                enabled: vm ? !vm.busy : false
                onClicked: {
                    if (vm) {
                        vm.loadLogs(vm.selectedLog)
                    }
                }
            }
            Button {
                text: qsTr("Clear")
                implicitHeight: 34
                implicitWidth: 90
                enabled: vm ? !vm.busy : false
                onClicked: {
                    if (vm) {
                        vm.clearLogs(vm.selectedLog)
                    }
                }
            }
            CheckBox {
                text: qsTr("Tail")
                checked: vm ? vm.isTailing : false
                enabled: vm ? !vm.busy : false
                onToggled: {
                    if (vm) {
                        vm.setTailing(checked)
                    }
                }
            }
            CheckBox {
                text: qsTr("Wrap")
                checked: vm ? vm.wrapLines : false
                enabled: vm ? !vm.busy : false
                onToggled: {
                    if (vm) {
                        vm.setWrapLines(checked)
                    }
                }
            }
            CheckBox {
                text: qsTr("Color")
                checked: vm ? vm.colorLines : false
                enabled: vm ? !vm.busy : false
                onToggled: {
                    if (vm) {
                        vm.setColorLines(checked)
                    }
                }
            }
        }

        Frame {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ScrollView {
                anchors.fill: parent
                clip: true

                TextArea {
                    id: logViewer
                    readOnly: true
                    wrapMode: vm && vm.wrapLines ? TextEdit.WrapAnywhere : TextEdit.NoWrap
                    textFormat: vm && vm.colorLines ? TextEdit.RichText : TextEdit.PlainText
                    text: vm ? vm.logText : ""
                    selectByMouse: true
                    font.family: "Noto Sans Mono"
                    color: Theme.textPrimary
                    background: Rectangle { color: Theme.surfaceVariant }
                }
            }

            Rectangle {
                anchors.fill: parent
                color: Theme.surfaceVariant
                opacity: vm ? (vm.busy ? 0.55 : 0) : 0
                visible: vm ? vm.busy : false
            }
            BusyIndicator {
                anchors.centerIn: parent
                running: vm ? vm.busy : false
                visible: running
            }
            Label {
                anchors.centerIn: parent
                text: vm && vm.busy ? vm.busyReason : ""
                color: Theme.textPrimary
                visible: vm ? (vm.busy && vm.busyReason.length > 0) : false
            }
        }
    }
}
