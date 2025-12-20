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
    id: otherLogsPage
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

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 0
        spacing: 0

        // Top bar: log selection
        RowLayout {
            Layout.fillWidth: true
            Layout.margins: Theme.spacingS
            spacing: Theme.spacingM

            ComboBox {
                id: selectLogBox
                Layout.fillWidth: true
                model: vm ? vm.otherLogsList : []
                currentIndex: 0
                onCurrentIndexChanged: {
                    if (vm && currentIndex >= 0) {
                        vm.loadOtherLog(currentIndex);
                    }
                }
            }

            Button {
                text: qsTr("Delete Selected")
                ToolTip.visible: hovered
                ToolTip.text: qsTr("Delete the selected log")
                onClicked: {
                    if (vm && selectLogBox.currentIndex >= 0) {
                        vm.deleteSelectedLog(selectLogBox.currentIndex);
                    }
                }
            }

            Button {
                text: qsTr("Delete All")
                ToolTip.visible: hovered
                ToolTip.text: qsTr("Delete all the logs")
                onClicked: {
                    if (vm)
                        vm.deleteAllLogs();
                }
            }
        }

        // Options row
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
                text: qsTr("Copy")
                ToolTip.visible: hovered
                ToolTip.text: qsTr("Copy the whole log into the clipboard")
                onClicked: {
                    if (vm)
                        vm.copyOtherLogToClipboard();
                }
            }

            Button {
                text: qsTr("Upload")
                ToolTip.visible: hovered
                ToolTip.text: qsTr("Upload the log to the paste service configured in preferences")
                onClicked: {
                    if (vm)
                        vm.uploadOtherLog();
                }
            }

            Button {
                text: qsTr("Reload")
                ToolTip.visible: hovered
                ToolTip.text: qsTr("Reload the contents of the log from the disk")
                onClicked: {
                    if (vm)
                        vm.reloadOtherLog();
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
                text: vm ? vm.otherLogContent : ""
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
                    otherLogsPage.findNext();
                }
            }

            Button {
                text: qsTr("Find")
                onClicked: {
                    otherLogsPage.findNext();
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
            function onOtherLogContentChanged() {
                if (trackLogCheckbox.checked) {
                    logScrollView.ScrollBar.vertical.position = 1.0 - logScrollView.ScrollBar.vertical.size;
                }
            }
        }
    }
}
