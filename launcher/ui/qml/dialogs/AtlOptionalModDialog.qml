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
import "../../Theme.js" as Theme

WindowDialog {
    id: atlModDialog
    title: qsTr("Select Mods To Install")
    modal: true
    width: 550
    height: 400
    standardButtons: Dialog.NoButton

    property var optionalMods: []  // Array of {name, description, recommended, selected}
    property string shareCode: ""

    signal installRequested(var selectedMods)

    ColumnLayout {
        anchors.fill: parent
        spacing: Theme.spacingS

        // Mod list
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: ThemeColors.backgroundAlt
            border.color: ThemeColors.border
            radius: Theme.radiusS

            TreeView {
                id: modTree
                anchors.fill: parent
                anchors.margins: 1
                clip: true
                model: optionalMods

                delegate: ItemDelegate {
                    width: modTree.width
                    height: 50

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: Theme.spacingS
                        spacing: Theme.spacingM

                        CheckBox {
                            id: modCheckbox
                            checked: modelData.selected || modelData.recommended
                            onCheckedChanged: {
                                var mods = optionalMods.slice();
                                mods[index].selected = checked;
                                optionalMods = mods;
                            }
                        }

                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2

                            RowLayout {
                                spacing: Theme.spacingS

                                Label {
                                    text: modelData.name || ""
                                    color: ThemeColors.text
                                    font.bold: true
                                }

                                Label {
                                    text: qsTr("(Recommended)")
                                    color: ThemeColors.accent
                                    font.pixelSize: 10
                                    visible: modelData.recommended
                                }
                            }

                            Label {
                                Layout.fillWidth: true
                                text: modelData.description || ""
                                color: ThemeColors.textSecondary
                                font.pixelSize: 11
                                elide: Text.ElideRight
                            }
                        }
                    }
                }

                ScrollBar.vertical: ScrollBar {}
            }
        }

        // Buttons
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingS

            Button {
                text: qsTr("Use Share Code")
                onClicked: shareCodeDialog.open()
            }

            Button {
                text: qsTr("Select Recommended")
                onClicked: {
                    var mods = optionalMods.slice();
                    for (var i = 0; i < mods.length; i++) {
                        mods[i].selected = mods[i].recommended;
                    }
                    optionalMods = mods;
                }
            }

            Button {
                text: qsTr("Clear All")
                onClicked: {
                    var mods = optionalMods.slice();
                    for (var i = 0; i < mods.length; i++) {
                        mods[i].selected = false;
                    }
                    optionalMods = mods;
                }
            }

            Item {
                Layout.fillWidth: true
            }

            Button {
                text: qsTr("Install")
                highlighted: true
                onClicked: {
                    var selected = [];
                    for (var i = 0; i < optionalMods.length; i++) {
                        if (optionalMods[i].selected) {
                            selected.push(optionalMods[i]);
                        }
                    }
                    installRequested(selected);
                    atlModDialog.accept();
                }
            }
        }
    }

    // Share code dialog
    Dialog {
        id: shareCodeDialog
        title: qsTr("Enter Share Code")
        modal: true
        standardButtons: Dialog.Ok | Dialog.Cancel
        width: 300

        ColumnLayout {
            anchors.fill: parent

            Label {
                text: qsTr("Enter the share code to apply mod selections:")
                color: ThemeColors.text
                wrapMode: Text.WordWrap
            }

            TextField {
                id: shareCodeField
                Layout.fillWidth: true
                placeholderText: qsTr("Share code...")
            }
        }

        onAccepted: {
            shareCode = shareCodeField.text;
            // Apply share code logic would be handled by ViewModel
        }
    }
}
