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
    objectName: "logs"
    color: ThemeColors.background
    width: parent ? parent.width : 640
    height: parent ? parent.height : 480
    readonly property var vm: ProjT.logsVM

    function findNext() {
        if (!logViewer.text || !searchField.text)
            return
        var start = Math.max(0, logViewer.selectionStart + logViewer.selectedText.length)
        var idx = logViewer.text.indexOf(searchField.text, start)
        if (idx === -1) {
            idx = logViewer.text.indexOf(searchField.text, 0)
        }
        if (idx >= 0) {
            logViewer.select(idx, idx + searchField.text.length)
            logViewer.cursorPosition = idx + searchField.text.length
        }
    }

    function scrollToBottom() {
        logViewer.cursorPosition = logViewer.length
        logViewer.select(logViewer.length, logViewer.length)
    }

    function copyLog() {
        if (!logViewer.text || logViewer.text.length === 0)
            return
        if (logViewer.selectedText && logViewer.selectedText.length > 0) {
            logViewer.copy()
        } else {
            logViewer.select(0, logViewer.length)
            logViewer.copy()
            logViewer.select(0, 0)
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingS
        spacing: Theme.spacingS
        Component.onCompleted: {
            if (vm) {
                if (vm.logList.length > 0 && vm.selectedLog === "") {
                    vm.selectedLog = vm.logList[0];
                }
                vm.loadLogs(vm.selectedLog);
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingS

            ComboBox {
                id: logSelector
                Layout.fillWidth: true
                model: vm ? vm.logList : []
                textRole: ""
                onActivated: {
                    if (vm && currentIndex >= 0 && currentIndex < model.length) {
                        vm.selectedLog = model[currentIndex];
                        vm.loadLogs(vm.selectedLog);
                    }
                }
                Component.onCompleted: {
                    if (vm && vm.logList.length > 0 && vm.selectedLog === "") {
                        vm.selectedLog = vm.logList[0];
                        vm.loadLogs(vm.selectedLog);
                    }
                }
                delegate: ItemDelegate {
                    width: logSelector.width
                    text: modelData
                    highlighted: logSelector.currentIndex === index
                    onClicked: {
                        logSelector.currentIndex = index;
                        logSelector.activated(index);
                    }
                }
            }

            Button {
                text: qsTr("Delete Selected")
                enabled: vm ? !vm.busy : false
                onClicked: {
                    if (vm) {
                        vm.clearLogs(vm.selectedLog);
                    }
                }
            }
            Button {
                text: qsTr("Delete All")
                enabled: vm ? !vm.busy : false
                onClicked: {
                    if (vm && vm.logList) {
                        for (var i = 0; i < vm.logList.length; ++i) {
                            vm.clearLogs(vm.logList[i]);
                        }
                    }
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingS

            CheckBox {
                text: qsTr("Keep updating")
                checked: vm ? vm.tailing : true
                enabled: vm ? !vm.busy : false
                onToggled: {
                    if (!vm)
                        return;
                    if (checked) {
                        vm.tailLogs(vm.selectedLog);
                    } else {
                        vm.tailing = false;
                    }
                }
            }
            CheckBox {
                text: qsTr("Wrap lines")
                checked: vm ? vm.wrapLines : true
                enabled: vm ? !vm.busy : false
                onToggled: {
                    if (vm) {
                        vm.setWrapLines(checked);
                    }
                }
            }
            CheckBox {
                text: qsTr("Color lines")
                checked: vm ? vm.colorLines : true
                enabled: vm ? !vm.busy : false
                onToggled: {
                    if (vm) {
                        vm.setColorLines(checked);
                    }
                }
            }

            Item {
                Layout.fillWidth: true
            }

            Button {
                text: qsTr("Copy")
                enabled: vm ? (vm.logText && vm.logText.length > 0 && !vm.busy) : false
                onClicked: {
                    copyLog()
                }
            }
            Button {
                text: qsTr("Upload")
                enabled: false
            }
            Button {
                text: qsTr("Reload")
                enabled: vm ? !vm.busy : false
                onClicked: {
                    if (vm) {
                        vm.loadLogs(vm.selectedLog);
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
                    wrapMode: vm ? (vm.wrapLines ? TextEdit.WrapAnywhere : TextEdit.NoWrap) : TextEdit.WrapAnywhere
                    textFormat: vm && vm.colorLines ? TextEdit.RichText : TextEdit.PlainText
                    text: vm ? vm.logText : ""
                    selectByMouse: true
                    font.family: "Noto Sans Mono"
                    color: ThemeColors.text
                    background: Rectangle {
                        color: ThemeColors.backgroundAlt
                    }
                }
            }

            Rectangle {
                anchors.fill: parent
                color: ThemeColors.backgroundAlt
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
                color: ThemeColors.text
                visible: vm ? (vm.busy && vm.busyReason.length > 0) : false
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingS

            TextField {
                id: searchField
                Layout.fillWidth: true
                placeholderText: qsTr("Search")
                onAccepted: findNext()
            }

            Button {
                text: qsTr("Find")
                enabled: logViewer.text.length > 0 && searchField.text.length > 0
                onClicked: findNext()
            }

            Rectangle {
                width: 1
                height: parent.height
                color: ThemeColors.border
            }

            Button {
                text: qsTr("Bottom")
                enabled: vm ? !vm.busy : true
                onClicked: scrollToBottom()
            }
        }
    }
}
