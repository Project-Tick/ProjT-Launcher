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

Rectangle {
    id: javaPage
    color: ThemeColors.background

    property var vm: ProjT.settingsVM
    property string javaPath: ""
    property bool autoDetect: true

    signal javaPathEdited(string path)
    signal autoDetectToggled(bool enabled)

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingL
        spacing: Theme.spacingM

        // Title
        Label {
            text: qsTr("Java Configuration")
            font.pixelSize: 18
            font.bold: true
            color: ThemeColors.text
        }

        // Description
        Label {
            Layout.fillWidth: true
            text: qsTr("Minecraft requires Java to run. You can let the launcher automatically detect and manage Java installations, or specify a custom Java path.")
            color: ThemeColors.text
            wrapMode: Text.WordWrap
        }

        // Separator
        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: ThemeColors.border
        }

        // Auto-detect option
        CheckBox {
            id: autoDetectCheckbox
            text: qsTr("Automatically detect Java")
            checked: autoDetect
            onCheckedChanged: {
                autoDetect = checked;
                autoDetectToggled(checked);
            }
        }

        // Custom path section
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Custom Java Path")
            enabled: !autoDetectCheckbox.checked
            opacity: enabled ? 1.0 : 0.5

            background: Rectangle {
                y: parent.topPadding - parent.padding
                width: parent.width
                height: parent.height - parent.topPadding + parent.padding
                color: "transparent"
                border.color: ThemeColors.border
                radius: Theme.radiusS
            }

            label: Label {
                x: Theme.spacingS
                text: parent.title
                color: ThemeColors.text
            }

            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS

                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingS

                    TextField {
                        id: javaPathField
                        Layout.fillWidth: true
                        text: javaPath
                        placeholderText: qsTr("Path to Java executable...")
                        onTextChanged: {
                            javaPath = text;
                            javaPathEdited(text);
                        }
                    }

                    ThemedButton {
                        text: qsTr("Browse...")
                        outline: true
                        onClicked: {
                            if (vm)
                                vm.browseJavaPath();
                        }
                    }

                    ThemedButton {
                        text: qsTr("Detect")
                        primary: true
                        onClicked: {
                            if (vm)
                                vm.detectJavaInstallations();
                        }
                    }
                }

                // Detected Java installations
                Label {
                    text: qsTr("Detected Java installations:")
                    color: ThemeColors.text
                    visible: javaList.count > 0
                }

                ListView {
                    id: javaList
                    Layout.fillWidth: true
                    Layout.preferredHeight: Math.min(count * 40, 160)
                    visible: count > 0
                    clip: true
                    model: vm ? vm.detectedJavaList : []

                    delegate: ItemDelegate {
                        width: javaList.width
                        height: 40

                        contentItem: RowLayout {
                            spacing: Theme.spacingS

                            Label {
                                text: modelData.version || "Java"
                                color: ThemeColors.text
                            }

                            Label {
                                Layout.fillWidth: true
                                text: modelData.path || modelData
                                color: ThemeColors.textSecondary
                                elide: Text.ElideMiddle
                            }
                        }

                        onClicked: {
                            javaPathField.text = modelData.path || modelData;
                        }
                    }

                    ScrollBar.vertical: ScrollBar {}
                }
            }
        }

        // Test button
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingM

            ThemedButton {
                text: qsTr("Test Java")
                success: true
                enabled: javaPath.length > 0 || autoDetect
                onClicked: {
                    if (vm)
                        vm.testJavaSettings();
                }
            }

            Label {
                id: testResultLabel
                color: ThemeColors.success
                visible: false
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }

    Connections {
        target: vm
        ignoreUnknownSignals: true

        function onJavaPathSelected(path) {
            javaPathField.text = path;
        }

        function onJavaTestResult(success, message) {
            testResultLabel.text = message;
            testResultLabel.color = success ? ThemeColors.success : ThemeColors.error;
            testResultLabel.visible = true;
        }
    }
}
