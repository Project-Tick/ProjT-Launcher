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
import "../components"

WindowDialog {
    id: createShortcutDialog
    title: qsTr("Create Shortcut")
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel
    width: 400
    height: 300

    property var instance: null
    property string shortcutName: ""
    property string shortcutLocation: "desktop"
    property bool launchDirectly: true

    ColumnLayout {
        anchors.fill: parent
        spacing: Theme.spacingM

        // Instance info
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingM

            Rectangle {
                Layout.preferredWidth: 48
                Layout.preferredHeight: 48
                radius: 8
                color: ThemeColors.backgroundAlt

                Image {
                    anchors.fill: parent
                    anchors.margins: 4
                    source: instance && instance.iconPath ? instance.iconPath : ""
                    fillMode: Image.PreserveAspectFit
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 2

                Label {
                    text: instance && instance.name ? instance.name : qsTr("No instance selected")
                    color: ThemeColors.text
                    font.bold: true
                }

                Label {
                    text: instance && instance.version ? instance.version : ""
                    color: ThemeColors.textSecondary
                    font.pointSize: Theme.fontSizeSmall
                }
            }
        }

        // Shortcut name
    GroupBox {
        Layout.fillWidth: true
        title: qsTr("Shortcut Settings")

            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS

                RowLayout {
                    Layout.fillWidth: true

                    Label {
                        text: qsTr("Name:")
                        Layout.preferredWidth: 80
                    }

                    TextField {
                        id: nameField
                        Layout.fillWidth: true
                        text: shortcutName
                        placeholderText: qsTr("Shortcut name...")
                        onTextChanged: shortcutName = text
                    }
                }

                RowLayout {
                    Layout.fillWidth: true

                    Label {
                        text: qsTr("Location:")
                        Layout.preferredWidth: 80
                    }

                    ComboBox {
                        id: locationCombo
                        Layout.fillWidth: true
                        model: [
                            {
                                text: qsTr("Desktop"),
                                value: "desktop"
                            },
                            {
                                text: qsTr("Start Menu"),
                                value: "startmenu"
                            },
                            {
                                text: qsTr("Applications"),
                                value: "applications"
                            }
                        ]
                        textRole: "text"
                        valueRole: "value"
                        currentIndex: 0
                        onCurrentValueChanged: shortcutLocation = currentValue
                    }
                }
            }
        }

        // Options
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Options")

            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS

                CheckBox {
                    text: qsTr("Launch game directly (skip launcher window)")
                    checked: launchDirectly
                    onCheckedChanged: launchDirectly = checked
                }

                Label {
                    Layout.fillWidth: true
                    text: qsTr("When enabled, double-clicking the shortcut will launch the game immediately.")
                    color: ThemeColors.textSecondary
                    font.pointSize: Theme.fontSizeSmall
                    wrapMode: Text.WordWrap
                }
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }

    onAccepted: {
        if (instance && ProjT && ProjT.instancesVM) {
            ProjT.instancesVM.createShortcut(instance.id, nameField.text, locationCombo.currentValue, launchDirectly);
        }
    }

    onOpened: {
        shortcutName = instance && instance.name ? instance.name : "";
        if (!shortcutLocation) {
            shortcutLocation = "desktop";
        }

        var idx = 0;
        for (var i = 0; i < locationCombo.model.length; i++) {
            if (locationCombo.model[i].value === shortcutLocation) {
                idx = i;
                break;
            }
        }
        locationCombo.currentIndex = idx;
        nameField.selectAll();
    }
}
