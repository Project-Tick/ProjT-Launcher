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
import ProjTLauncher 1.0
import "../Theme.js" as Theme

/**
 * SubTaskProgressBar - Progress indicator for sub-tasks with status text
 */
Rectangle {
    id: subTaskProgress
    color: "transparent"

    property string statusText: ""
    property string statusDetails: ""
    property real progress: 0  // 0.0 to 1.0
    property bool indeterminate: false

    implicitHeight: 50
    implicitWidth: 300

    ColumnLayout {
        anchors.fill: parent
        spacing: 2

        // Status row
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingM

            Label {
                Layout.fillWidth: true
                text: statusText
                color: ThemeColors.text
                font.pixelSize: 11
                wrapMode: Text.WordWrap
                elide: Text.ElideRight
                maximumLineCount: 2
            }

            Label {
                text: statusDetails
                color: ThemeColors.textSecondary
                font.pixelSize: 11
                horizontalAlignment: Text.AlignRight
            }
        }

        // Progress bar
        ProgressBar {
            id: progressBar
            Layout.fillWidth: true
            Layout.preferredHeight: 20
            value: progress
            indeterminate: subTaskProgress.indeterminate

            background: Rectangle {
                implicitHeight: 20
                color: ThemeColors.backgroundAlt
                radius: Theme.radiusS
            }

            contentItem: Item {
                Rectangle {
                    width: progressBar.indeterminate ? parent.width * 0.3 : progressBar.visualPosition * parent.width
                    height: parent.height
                    radius: Theme.radiusS
                    color: ThemeColors.accent

                    // Indeterminate animation
                    SequentialAnimation on x {
                        running: progressBar.indeterminate
                        loops: Animation.Infinite
                        NumberAnimation {
                            from: 0
                            to: progressBar.width * 0.7
                            duration: 1000
                            easing.type: Easing.InOutQuad
                        }
                        NumberAnimation {
                            from: progressBar.width * 0.7
                            to: 0
                            duration: 1000
                            easing.type: Easing.InOutQuad
                        }
                    }
                }

                // Percentage text
                Label {
                    anchors.centerIn: parent
                    text: progressBar.indeterminate ? "" : Math.round(progress * 100) + "%"
                    color: progress > 0.5 ? "white" : ThemeColors.text
                    font.pixelSize: 10
                    font.bold: true
                    visible: !progressBar.indeterminate
                }
            }
        }
    }
}
