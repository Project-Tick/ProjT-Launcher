// SPDX-License-Identifier: GPL-3.0-or-later
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
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import ProjTLauncher 1.0
import "../../Theme.js" as Theme

Dialog {
    id: optionalModDialog
    title: qsTr("Select Optional Mods")
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel
    width: 550
    height: 350

    property var optionalMods: []  // Array of {name: "", description: "", checked: true}

    signal modsSelected(var selectedMods)

    ColumnLayout {
        anchors.fill: parent
        spacing: Theme.spacingM

        // Mod list
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: ThemeColors.backgroundAlt
            border.color: ThemeColors.border
            radius: Theme.radiusS

            ListView {
                id: modList
                anchors.fill: parent
                anchors.margins: 1
                clip: true
                model: optionalMods

                delegate: Rectangle {
                    width: modList.width
                    height: 50
                    color: index % 2 === 0 ? "transparent" : ThemeColors.surface

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: Theme.spacingS
                        spacing: Theme.spacingM

                        CheckBox {
                            id: modCheckbox
                            checked: modelData.checked !== false
                            onCheckedChanged: {
                                var mods = optionalMods.slice();
                                mods[index].checked = checked;
                                optionalMods = mods;
                            }
                        }

                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2

                            Label {
                                text: modelData.name || ""
                                color: ThemeColors.text
                                font.bold: true
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

        // Selection buttons
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingM

            Button {
                text: qsTr("Select All")
                onClicked: {
                    var mods = optionalMods.slice();
                    for (var i = 0; i < mods.length; i++) {
                        mods[i].checked = true;
                    }
                    optionalMods = mods;
                }
            }

            Button {
                text: qsTr("Deselect All")
                onClicked: {
                    var mods = optionalMods.slice();
                    for (var i = 0; i < mods.length; i++) {
                        mods[i].checked = false;
                    }
                    optionalMods = mods;
                }
            }

            Item {
                Layout.fillWidth: true
            }

            Label {
                text: qsTr("Unchecked mods will be disabled.")
                color: ThemeColors.textSecondary
                font.italic: true
            }
        }
    }

    onAccepted: {
        var selected = [];
        for (var i = 0; i < optionalMods.length; i++) {
            if (optionalMods[i].checked) {
                selected.push(optionalMods[i]);
            }
        }
        modsSelected(selected);
    }
}
