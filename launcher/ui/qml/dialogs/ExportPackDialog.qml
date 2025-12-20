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

Dialog {
    id: exportPackDialog
    title: qsTr("Export Pack")
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel
    width: 650
    height: 550

    property var vm: ProjT.instancesVM
    property string instanceId: ""

    // Pack description
    property string packName: ""
    property string packVersion: "1.0.0"
    property string packAuthor: ""
    property string packSummary: ""
    property int recommendedMemory: 4096
    property bool includeRecommendedMemory: false
    property bool markOptionalFiles: true

    ColumnLayout {
        anchors.fill: parent
        spacing: Theme.spacingM

        // Description GroupBox
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Description")

            background: Rectangle {
                y: parent.topPadding - parent.padding
                width: parent.width
                height: parent.height - parent.topPadding + parent.padding
                color: "transparent"
                border.color: ThemeColors.border
                radius: Theme.radiusS
            }

            label: Label {
                x: Theme.spacingM
                text: parent.title
                color: ThemeColors.text
            }

            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS

                GridLayout {
                    columns: 2
                    rowSpacing: Theme.spacingS
                    columnSpacing: Theme.spacingM

                    Label {
                        text: qsTr("Name:")
                        color: ThemeColors.text
                    }
                    TextField {
                        id: nameField
                        Layout.fillWidth: true
                        text: packName
                        onTextChanged: packName = text
                    }

                    Label {
                        text: qsTr("Version:")
                        color: ThemeColors.text
                    }
                    TextField {
                        id: versionField
                        Layout.fillWidth: true
                        text: packVersion
                        onTextChanged: packVersion = text
                    }

                    Label {
                        text: qsTr("Author:")
                        color: ThemeColors.text
                    }
                    TextField {
                        id: authorField
                        Layout.fillWidth: true
                        text: packAuthor
                        onTextChanged: packAuthor = text
                    }
                }

                Label {
                    text: qsTr("Summary:")
                    color: ThemeColors.text
                }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 80

                    TextArea {
                        id: summaryField
                        text: packSummary
                        wrapMode: TextEdit.Wrap
                        placeholderText: qsTr("Enter a short description of the pack...")
                        onTextChanged: packSummary = text
                        background: Rectangle {
                            color: ThemeColors.backgroundAlt
                            border.color: ThemeColors.border
                            radius: Theme.radiusS
                        }
                    }
                }
            }
        }

        // Options GroupBox
        GroupBox {
            Layout.fillWidth: true
            Layout.fillHeight: true
            title: qsTr("Options")

            background: Rectangle {
                y: parent.topPadding - parent.padding
                width: parent.width
                height: parent.height - parent.topPadding + parent.padding
                color: "transparent"
                border.color: ThemeColors.border
                radius: Theme.radiusS
            }

            label: Label {
                x: Theme.spacingM
                text: parent.title
                color: ThemeColors.text
            }

            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS

                // Recommended memory
                RowLayout {
                    spacing: Theme.spacingM

                    CheckBox {
                        id: memoryCheckBox
                        text: qsTr("Recommended Memory:")
                        checked: includeRecommendedMemory
                        onCheckedChanged: includeRecommendedMemory = checked
                    }

                    SpinBox {
                        enabled: memoryCheckBox.checked
                        from: 512
                        to: 32768
                        stepSize: 128
                        value: recommendedMemory
                        onValueChanged: recommendedMemory = value

                        textFromValue: function (value) {
                            return value + " MiB";
                        }
                        valueFromText: function (text) {
                            return parseInt(text);
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                    }
                }

                Label {
                    text: qsTr("Files:")
                    color: ThemeColors.text
                }

                // File tree
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: ThemeColors.backgroundAlt
                    border.color: ThemeColors.border
                    radius: Theme.radiusS

                    TreeView {
                        id: filesTree
                        anchors.fill: parent
                        anchors.margins: 1
                        clip: true
                        model: vm ? vm.exportFilesModel : null
                        alternatingRows: true

                        delegate: Item {
                            implicitWidth: filesTree.width
                            implicitHeight: 30

                            CheckBox {
                                anchors.left: parent.left
                                anchors.leftMargin: depth * 20 + Theme.spacingS
                                anchors.verticalCenter: parent.verticalCenter
                                text: model.name || ""
                                checked: model.checked !== false
                                onCheckedChanged: model.checked = checked
                            }
                        }

                        ScrollBar.vertical: ScrollBar {}
                    }
                }

                CheckBox {
                    text: qsTr("Mark disabled files as optional")
                    checked: markOptionalFiles
                    onCheckedChanged: markOptionalFiles = checked
                }
            }
        }
    }

    onAccepted: {
        if (vm) {
            vm.exportPack(instanceId, {
                name: packName,
                version: packVersion,
                author: packAuthor,
                summary: packSummary,
                recommendedMemory: includeRecommendedMemory ? recommendedMemory : -1,
                markOptionalFiles: markOptionalFiles
            });
        }
    }
}
