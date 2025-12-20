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
import "../Theme.js" as Theme

Rectangle {
    id: logPage
    color: ThemeColors.background

    property var vm: ProjT.instanceVM

    function findNext() {
        if (!logText.text || !searchBar.text)
            return;
        var start = Math.max(0, logText.selectionStart + logText.selectedText.length);
        var idx = logText.text.indexOf(searchBar.text, start);
        if (idx === -1) {
            idx = logText.text.indexOf(searchBar.text, 0);
        }
        if (idx >= 0) {
            logText.select(idx, idx + searchBar.text.length);
            logText.cursorPosition = idx + searchBar.text.length;
        }
    }

    Component.onCompleted: {
        if (vm) {
            vm.refreshInstanceLog()
        }
    }

    Connections {
        target: vm
        ignoreUnknownSignals: true
        function onInstanceIdChanged() {
            if (vm) {
                vm.refreshInstanceLog()
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 0
        spacing: 0

        // Top toolbar
        RowLayout {
            Layout.fillWidth: true
            Layout.margins: Theme.spacingS
            spacing: Theme.spacingM

            CheckBox {
                id: trackLogCheckbox
                text: qsTr("Keep updating")
                checked: true
            }

            CheckBox {
                id: wrapCheckbox
                text: qsTr("Wrap lines")
                checked: true
            }

            CheckBox {
                id: colorCheckbox
                text: qsTr("Color lines")
                checked: true
            }

            Item {
                Layout.fillWidth: true
            }

            Button {
                text: qsTr("&Copy")
                ToolTip.visible: hovered
                ToolTip.text: qsTr("Copy the whole log into the clipboard")
                onClicked: {
                    if (vm)
                        vm.copyLogToClipboard();
                }
            }

            Button {
                text: qsTr("Upload")
                ToolTip.visible: hovered
                ToolTip.text: qsTr("Upload the log to the paste service configured in preferences")
                onClicked: {
                    if (vm)
                        vm.uploadLog();
                }
            }

            Button {
                text: qsTr("Clear")
                ToolTip.visible: hovered
                ToolTip.text: qsTr("Clear the log")
                onClicked: {
                    if (vm)
                        vm.clearLog();
                }
            }
        }

        Timer {
            id: logRefreshTimer
            interval: 1000
            running: trackLogCheckbox.checked
            repeat: true
            onTriggered: {
                if (vm) {
                    vm.refreshInstanceLog()
                }
            }
        }

        // Log viewer
        ScrollView {
            id: logScrollView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            TextArea {
                id: logText
                readOnly: true
                wrapMode: wrapCheckbox.checked ? TextEdit.WrapAnywhere : TextEdit.NoWrap
                text: vm ? vm.instanceLog : ""
                font.family: "Noto Sans Mono"
                font.pointSize: 10
                color: ThemeColors.text
                selectByMouse: true
                textFormat: colorCheckbox.checked ? TextEdit.RichText : TextEdit.PlainText

                background: Rectangle {
                    color: ThemeColors.backgroundAlt
                }
            }
        }

        // Bottom search bar
        RowLayout {
            Layout.fillWidth: true
            Layout.margins: Theme.spacingS
            spacing: Theme.spacingS

            TextField {
                id: searchBar
                Layout.fillWidth: true
                placeholderText: qsTr("Search")
                onAccepted: {
                    logPage.findNext();
                }
            }

            Button {
                text: qsTr("Find")
                onClicked: {
                    logPage.findNext();
                }
            }

            Rectangle {
                width: 1
                height: parent.height - 8
                color: ThemeColors.border
            }

            Button {
                text: qsTr("Bottom")
                ToolTip.visible: hovered
                ToolTip.text: qsTr("Scroll all the way to bottom")
                onClicked: {
                    logScrollView.ScrollBar.vertical.position = 1.0 - logScrollView.ScrollBar.vertical.size;
                }
            }
        }

        // Auto-scroll on new content
        Connections {
            target: vm
            ignoreUnknownSignals: true
            function onInstanceLogChanged() {
                if (trackLogCheckbox.checked) {
                    logScrollView.ScrollBar.vertical.position = 1.0 - logScrollView.ScrollBar.vertical.size;
                }
            }
        }
    }
}
