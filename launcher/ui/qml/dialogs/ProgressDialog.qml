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

WindowDialog {
    id: progressDialog
    title: qsTr("Please wait...")
    modal: true
    closePolicy: Popup.NoAutoClose
    width: 480
    height: 280
    standardButtons: Dialog.Cancel

    property string globalStatus: ""
    property string globalStatusDetails: ""
    property real globalProgress: 0
    property bool globalIndeterminate: false
    property var subtasks: []  // Array of {name, status, progress}
    property bool showSkipButton: false
    property var vm: null

    ColumnLayout {
        anchors.fill: parent
        spacing: Theme.spacingS

        // Global status
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingS

            Label {
                text: globalStatus || qsTr("Global Task Status...")
                color: ThemeColors.text
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
            }

            Label {
                text: globalStatusDetails || ""
                color: ThemeColors.textSecondary
                horizontalAlignment: Text.AlignRight
            }
        }

        // Global progress bar
        ProgressBar {
            id: globalProgressBar
            Layout.fillWidth: true
            Layout.minimumHeight: 24
            value: globalProgress
            indeterminate: globalIndeterminate
        }

        // Subtask scroll area
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: 100
            clip: true

            contentWidth: availableWidth

            ColumnLayout {
                width: parent.width
                spacing: 2

                Repeater {
                    model: subtasks

                    delegate: ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 2

                        RowLayout {
                            Layout.fillWidth: true

                            Label {
                                text: modelData.name || ""
                                color: ThemeColors.text
                                Layout.fillWidth: true
                                elide: Text.ElideRight
                            }

                            Label {
                                text: modelData.status || ""
                                color: ThemeColors.textSecondary
                            }
                        }

                        ProgressBar {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 16
                            value: modelData.progress || 0
                            indeterminate: modelData.indeterminate || false
                        }
                    }
                }
            }
        }

        // Skip button
        Button {
            Layout.fillWidth: true
            text: qsTr("Skip")
            visible: showSkipButton
            onClicked: {
                if (vm)
                    vm.skipTask();
            }
        }
    }

    onRejected: {
        if (vm)
            vm.cancelTask();
    }

    // Helper functions
    function updateGlobalProgress(status, details, progress, indeterminate) {
        globalStatus = status;
        globalStatusDetails = details;
        globalProgress = progress;
        globalIndeterminate = indeterminate;
    }

    function addSubtask(name, status, progress, indeterminate) {
        var newTask = {
            name: name,
            status: status,
            progress: progress,
            indeterminate: indeterminate || false
        };
        subtasks = subtasks.concat([newTask]);
    }

    function updateSubtask(index, status, progress) {
        if (index >= 0 && index < subtasks.length) {
            var updated = subtasks.slice();
            updated[index].status = status;
            updated[index].progress = progress;
            subtasks = updated;
        }
    }

    function clearSubtasks() {
        subtasks = [];
    }
}
