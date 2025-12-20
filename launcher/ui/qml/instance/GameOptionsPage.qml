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
    id: gameOptionsPage
    color: ThemeColors.background

    property var vm: ProjT.instanceVM

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingM
        spacing: Theme.spacingS

        // Header
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingS

            Label {
                text: qsTr("Game Options")
                font.pointSize: 14
                font.bold: true
                color: ThemeColors.text
            }

            Item {
                Layout.fillWidth: true
            }

            Button {
                text: qsTr("Refresh")
                icon.name: "view-refresh"
                onClicked: {
                    if (vm)
                        vm.refreshGameOptions();
                }
            }

            Button {
                text: qsTr("Reset to Defaults")
                onClicked: resetDialog.open()
            }
        }

        Label {
            text: qsTr("Edit Minecraft's options.txt file. Changes take effect on next game launch.")
            color: ThemeColors.textSecondary
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }

        // Search
        TextField {
            id: searchField
            Layout.fillWidth: true
            placeholderText: qsTr("Search options...")
            onTextChanged: {
                if (vm)
                    vm.filterGameOptions(text);
            }
        }

        // Options list
        Frame {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ListView {
                id: optionsList
                anchors.fill: parent
                clip: true
                model: vm ? vm.gameOptionsModel : []

                delegate: ItemDelegate {
                    width: optionsList.width
                    height: 40

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: Theme.spacingS
                        spacing: Theme.spacingS

                        Label {
                            text: model.key || ""
                            color: ThemeColors.text
                            Layout.preferredWidth: 200
                            elide: Text.ElideRight
                        }

                        TextField {
                            Layout.fillWidth: true
                            text: model.value || ""
                            onTextChanged: {
                                if (vm && text !== model.value) {
                                    vm.setGameOption(model.key, text);
                                }
                            }
                        }

                        ToolButton {
                            icon.name: "edit-undo"
                            ToolTip.text: qsTr("Reset to default")
                            ToolTip.visible: hovered
                            onClicked: {
                                if (vm)
                                    vm.resetGameOption(model.key);
                            }
                        }
                    }
                }

                ScrollBar.vertical: ScrollBar {}
            }

            Label {
                anchors.centerIn: parent
                visible: optionsList.count === 0
                text: qsTr("No game options found.\nRun the game once to generate options.txt")
                color: ThemeColors.textSecondary
                horizontalAlignment: Text.AlignHCenter
            }
        }

        // Actions
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingS

            Label {
                text: vm ? qsTr("%1 options").arg(vm.gameOptionsCount || 0) : ""
                color: ThemeColors.textSecondary
            }

            Item {
                Layout.fillWidth: true
            }

            Button {
                text: qsTr("Open options.txt")
                onClicked: {
                    if (vm)
                        vm.openOptionsFile();
                }
            }
        }
    }

    // Reset confirmation
    Dialog {
        id: resetDialog
        title: qsTr("Reset Game Options")
        modal: true
        standardButtons: Dialog.Yes | Dialog.No
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2

        Label {
            text: qsTr("Reset all game options to default values?\n\nThis will delete your options.txt file.")
            color: ThemeColors.error
            wrapMode: Text.WordWrap
        }

        onAccepted: {
            if (vm)
                vm.resetAllGameOptions();
        }
    }
}
