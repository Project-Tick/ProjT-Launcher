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

Rectangle {
    id: envVarsWidget
    color: "transparent"

    // Properties for external binding
    property bool overrideGlobalSettings: false
    property var environmentVariables: []  // Array of {name: "", value: ""}

    signal variablesChanged

    implicitHeight: mainLayout.implicitHeight

    ColumnLayout {
        id: mainLayout
        anchors.fill: parent
        spacing: Theme.spacingS

        // Override checkbox
        CheckBox {
            id: overrideCheckBox
            text: qsTr("Override Global Settings")
            checked: overrideGlobalSettings
            onCheckedChanged: overrideGlobalSettings = checked
        }

        // Variables container
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            enabled: overrideCheckBox.checked
            opacity: enabled ? 1.0 : 0.5
            spacing: Theme.spacingS

            // Toolbar
            RowLayout {
                Layout.fillWidth: true
                spacing: Theme.spacingS

                Button {
                    text: qsTr("Add")
                    onClicked: {
                        var newVars = environmentVariables.slice();
                        newVars.push({
                            name: "",
                            value: ""
                        });
                        environmentVariables = newVars;
                        variablesChanged();
                    }
                }

                Button {
                    text: qsTr("Remove")
                    enabled: varsList.currentIndex >= 0
                    onClicked: {
                        if (varsList.currentIndex >= 0) {
                            var newVars = environmentVariables.slice();
                            newVars.splice(varsList.currentIndex, 1);
                            environmentVariables = newVars;
                            variablesChanged();
                        }
                    }
                }

                Item {
                    Layout.fillWidth: true
                }

                Button {
                    text: qsTr("Clear")
                    enabled: environmentVariables.length > 0
                    onClicked: {
                        environmentVariables = [];
                        variablesChanged();
                    }
                }
            }

            // Variables list
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumHeight: 200
                color: ThemeColors.backgroundAlt
                border.color: ThemeColors.border
                radius: Theme.radiusS

                ListView {
                    id: varsList
                    anchors.fill: parent
                    anchors.margins: 1
                    clip: true
                    model: environmentVariables
                    currentIndex: -1

                    // Header
                    header: Rectangle {
                        width: varsList.width
                        height: 30
                        color: ThemeColors.surface

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: Theme.spacingS
                            anchors.rightMargin: Theme.spacingS
                            spacing: Theme.spacingM

                            Label {
                                Layout.preferredWidth: parent.width * 0.4
                                text: qsTr("Name")
                                font.bold: true
                                color: ThemeColors.text
                            }

                            Label {
                                Layout.fillWidth: true
                                text: qsTr("Value")
                                font.bold: true
                                color: ThemeColors.text
                            }
                        }
                    }

                    delegate: Rectangle {
                        width: varsList.width
                        height: 36
                        color: index === varsList.currentIndex ? ThemeColors.accent : (index % 2 === 0 ? "transparent" : ThemeColors.surface)

                        MouseArea {
                            anchors.fill: parent
                            onClicked: varsList.currentIndex = index
                        }

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: Theme.spacingS
                            anchors.rightMargin: Theme.spacingS
                            spacing: Theme.spacingM

                            TextField {
                                Layout.preferredWidth: parent.width * 0.4
                                text: modelData.name || ""
                                placeholderText: qsTr("VAR_NAME")
                                onTextChanged: {
                                    var newVars = environmentVariables.slice();
                                    newVars[index] = {
                                        name: text,
                                        value: newVars[index].value
                                    };
                                    environmentVariables = newVars;
                                    variablesChanged();
                                }
                            }

                            TextField {
                                Layout.fillWidth: true
                                text: modelData.value || ""
                                placeholderText: qsTr("value")
                                onTextChanged: {
                                    var newVars = environmentVariables.slice();
                                    newVars[index] = {
                                        name: newVars[index].name,
                                        value: text
                                    };
                                    environmentVariables = newVars;
                                    variablesChanged();
                                }
                            }
                        }
                    }

                    ScrollBar.vertical: ScrollBar {}
                }
            }
        }
    }
}
