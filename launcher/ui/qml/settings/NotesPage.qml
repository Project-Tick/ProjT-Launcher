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
    id: root
    objectName: "notesPage"
    color: ThemeColors.background

    property var vm: ProjT.instanceVM

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingM
        spacing: Theme.spacingM

        Text {
            text: qsTr("Notes")
            font.pixelSize: 24
            font.weight: Font.Bold
            color: ThemeColors.text
        }

        Text {
            text: root.vm && root.vm.instanceName ? root.vm.instanceName : qsTr("No instance selected")
            font.pixelSize: 14
            color: ThemeColors.textSecondary
            visible: root.vm !== null
        }

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            TextArea {
                id: notesTextArea
                placeholderText: qsTr("Add notes about this instance...")
                text: root.vm ? root.vm.notes : ""
                wrapMode: TextEdit.Wrap
                selectByMouse: true

                background: Rectangle {
                    color: ThemeColors.surface
                    border.color: notesTextArea.activeFocus ? ThemeColors.accent : ThemeColors.hover
                    border.width: 1
                    radius: Theme.radiusS
                }

                color: ThemeColors.text
                placeholderTextColor: ThemeColors.textSecondary
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingM

            Item {
                Layout.fillWidth: true
            }

            Button {
                text: qsTr("Clear")
                onClicked: notesTextArea.text = ""

                background: Rectangle {
                    color: parent.hovered ? ThemeColors.hover : ThemeColors.surface
                    border.color: ThemeColors.border
                    border.width: 1
                    radius: Theme.radiusS
                }

                contentItem: Text {
                    text: parent.text
                    color: ThemeColors.text
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }

            Button {
                text: qsTr("Save")
                onClicked: {
                    if (root.vm) {
                        root.vm.notes = notesTextArea.text;
                        root.vm.saveSettings();
                    }
                }

                background: Rectangle {
                    color: parent.hovered ? Qt.lighter(ThemeColors.accent, 1.1) : ThemeColors.accent
                    radius: Theme.radiusS
                }

                contentItem: Text {
                    text: parent.text
                    color: ThemeColors.surface
                    font.weight: Font.DemiBold
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
    }
}
