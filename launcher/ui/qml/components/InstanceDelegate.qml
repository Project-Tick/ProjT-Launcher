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
    id: delegate

    // Properties
    property string instanceId: ""
    property string instanceName: ""
    property string instanceGroup: ""
    property string iconPath: ""
    property bool isSelected: false
    property bool isRunning: false
    property string lastPlayedText: ""

    // Signals
    signal clicked(string instanceId)
    signal doubleClicked(string instanceId)
    signal rightClicked(string instanceId, int mouseX, int mouseY)

    Component.onCompleted: {
        console.log("[InstanceDelegate] Created - name:", instanceName, "id:", instanceId);
    }

    height: 56
    color: isSelected ? ThemeColors.highlight : (mouseArea.containsMouse ? ThemeColors.backgroundAlt : ThemeColors.surface)
    border.color: isSelected ? ThemeColors.accent : ThemeColors.border
    border.width: isSelected ? 1 : 0
    radius: Theme.radius

    Behavior on color {
        ColorAnimation {
            duration: 100
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingS
        spacing: Theme.spacingS

        // === Instance Icon ===
        Rectangle {
            width: 40
            height: 40
            radius: Theme.radius
            color: ThemeColors.backgroundAlt
            Layout.alignment: Qt.AlignVCenter

            Image {
                anchors.fill: parent
                anchors.margins: 2
                source: iconPath ? ("file://" + iconPath) : ""
                sourceSize: Qt.size(36, 36)
                fillMode: Image.PreserveAspectFit
                smooth: true

                // Fallback text if no icon
                Rectangle {
                    anchors.fill: parent
                    visible: !parent.status || parent.status === Image.Error
                    color: ThemeColors.backgroundAlt

                    Text {
                        anchors.centerIn: parent
                        text: instanceName.charAt(0).toUpperCase()
                        font.pointSize: 16
                        font.bold: true
                        color: ThemeColors.text
                    }
                }
            }
        }

        // === Instance Info ===
        ColumnLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingXS

            RowLayout {
                Layout.fillWidth: true
                spacing: Theme.spacingS

                Text {
                    text: instanceName
                    color: ThemeColors.text
                    font.pointSize: 12
                    font.bold: true
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }

                // Running indicator
                Rectangle {
                    width: 24
                    height: 24
                    radius: 12
                    visible: isRunning
                    color: ThemeColors.success

                    Text {
                        anchors.centerIn: parent
                        text: qsTr("R")
                        color: ThemeColors.highlightedText
                        font.pointSize: 10
                        font.bold: true
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: Theme.spacingS

                Text {
                    text: instanceGroup ? (instanceGroup) : qsTr("No Group")
                    color: ThemeColors.textSecondary
                    font.pointSize: 10
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }

                // Last played info
                Text {
                    text: lastPlayedText
                    color: ThemeColors.textSecondary
                    font.pointSize: 9
                    font.italic: true
                    Layout.alignment: Qt.AlignRight
                }
            }
        }

        // === Launch Button ===
        Button {
            id: playButton
            text: isRunning ? qsTr("Running...") : qsTr("Play")
            implicitHeight: 36
            implicitWidth: 56
            enabled: !isRunning
            Layout.alignment: Qt.AlignVCenter

            background: Rectangle {
                radius: Theme.radius
                color: playButton.enabled ? (playButton.hovered ? Qt.lighter(ThemeColors.success, 1.1) : ThemeColors.success) : ThemeColors.disabled
                border.color: Qt.darker(ThemeColors.success, 1.2)
                border.width: 1
            }

            contentItem: Text {
                text: playButton.text
                color: playButton.enabled ? ThemeColors.highlightedText : ThemeColors.textSecondary
                font.pointSize: 11
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            onClicked: delegate.doubleClicked(instanceId)
        }
    }

    // === Mouse Area for Selection & Context Menu ===
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.LeftButton | Qt.RightButton

        onClicked: function (mouse) {
            if (mouse.button === Qt.LeftButton) {
                delegate.clicked(instanceId);
            } else if (mouse.button === Qt.RightButton) {
                // Map to global screen coordinates for context menu
                var globalPos = mapToGlobal(mouse.x, mouse.y);
                delegate.rightClicked(instanceId, globalPos.x, globalPos.y);
            }
        }

        onDoubleClicked: function (mouse) {
            if (mouse.button === Qt.LeftButton) {
                delegate.doubleClicked(instanceId);
            }
        }
    }

    // === Tooltip ===
    ToolTip.text: qsTr("Left-click to select, double-click to launch, right-click for more options")
    ToolTip.visible: mouseArea.containsMouse && !isRunning
    ToolTip.delay: 800
}
