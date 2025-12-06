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
    id: exportModListDialog
    title: qsTr("Export Pack to ModList")
    modal: true
    width: 650
    height: 550

    property var vm: ProjT.instancesVM
    property string instanceId: ""

    // Export settings
    property int selectedFormat: 0  // 0=HTML, 1=Markdown, 2=Plaintext, 3=JSON, 4=CSV, 5=Custom
    property string customTemplate: "{name} - {version}"
    property bool includeVersion: false
    property bool includeAuthors: false
    property bool includeUrl: false
    property bool includeFilename: false

    readonly property var formatNames: ["HTML", "Markdown", "Plaintext", "JSON", "CSV", "Custom"]

    footer: DialogButtonBox {
        Button {
            text: qsTr("Copy")
            onClicked: {
                if (vm)
                    vm.copyToClipboard(resultText.text);
            }
        }
        Button {
            text: qsTr("Save")
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
        }
        Button {
            text: qsTr("Cancel")
            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: Theme.spacingM

        // Settings GroupBox
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Settings")

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

            GridLayout {
                anchors.fill: parent
                columns: 2
                rowSpacing: Theme.spacingS
                columnSpacing: Theme.spacingL

                // Left column: Format selection and template
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingS

                    RowLayout {
                        spacing: Theme.spacingM

                        Label {
                            text: qsTr("Format:")
                            color: ThemeColors.text
                        }

                        ComboBox {
                            id: formatCombo
                            Layout.fillWidth: true
                            model: formatNames
                            currentIndex: selectedFormat
                            onCurrentIndexChanged: {
                                selectedFormat = currentIndex;
                                updateResult();
                            }
                        }
                    }

                    // Template (only for Custom format)
                    GroupBox {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 120
                        title: qsTr("Template")
                        visible: selectedFormat === 5

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

                        ScrollView {
                            anchors.fill: parent

                            TextArea {
                                id: templateText
                                text: customTemplate
                                wrapMode: TextEdit.Wrap
                                onTextChanged: {
                                    customTemplate = text;
                                    updateResult();
                                }
                                placeholderText: qsTr("{name} - {version}\nPlaceholders: {name}, {mod_id}, {url}, {version}, {authors}")

                                ToolTip.visible: hovered
                                ToolTip.text: qsTr("Placeholders:\n{name} - Mod name\n{mod_id} - Mod ID\n{url} - Mod URL\n{version} - Mod version\n{authors} - Mod authors")

                                background: Rectangle {
                                    color: ThemeColors.backgroundAlt
                                    border.color: ThemeColors.border
                                    radius: Theme.radiusS
                                }
                            }
                        }
                    }
                }

                // Right column: Optional info
                GroupBox {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignTop
                    title: qsTr("Optional Info")
                    visible: selectedFormat !== 5

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
                        spacing: Theme.spacingXS

                        CheckBox {
                            text: qsTr("Version")
                            checked: includeVersion
                            onCheckedChanged: {
                                includeVersion = checked;
                                updateResult();
                            }
                        }

                        CheckBox {
                            text: qsTr("Authors")
                            checked: includeAuthors
                            onCheckedChanged: {
                                includeAuthors = checked;
                                updateResult();
                            }
                        }

                        CheckBox {
                            text: qsTr("URL")
                            checked: includeUrl
                            onCheckedChanged: {
                                includeUrl = checked;
                                updateResult();
                            }
                        }

                        CheckBox {
                            text: qsTr("Filename")
                            checked: includeFilename
                            onCheckedChanged: {
                                includeFilename = checked;
                                updateResult();
                            }
                        }
                    }
                }
            }
        }

        // Result GroupBox
        GroupBox {
            Layout.fillWidth: true
            Layout.fillHeight: true
            title: qsTr("Result")

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

            RowLayout {
                anchors.fill: parent
                spacing: Theme.spacingM

                // Plain text result
                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    TextArea {
                        id: resultText
                        readOnly: true
                        wrapMode: TextEdit.Wrap
                        selectByMouse: true
                        font.family: "Noto Sans Mono"
                        font.pointSize: 9
                        color: ThemeColors.text

                        background: Rectangle {
                            color: ThemeColors.backgroundAlt
                            border.color: ThemeColors.border
                            radius: Theme.radiusS
                        }
                    }
                }

                // Preview (for HTML/Markdown)
                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    visible: selectedFormat === 0 || selectedFormat === 1

                    TextArea {
                        id: previewText
                        readOnly: true
                        textFormat: TextEdit.RichText
                        wrapMode: TextEdit.Wrap
                        color: ThemeColors.text

                        background: Rectangle {
                            color: ThemeColors.backgroundAlt
                            border.color: ThemeColors.border
                            radius: Theme.radiusS
                        }

                        onLinkActivated: function (link) {
                            Qt.openUrlExternally(link);
                        }
                    }
                }
            }
        }

        // Warning label
        Label {
            Layout.fillWidth: true
            text: qsTr("This depends on the mods' metadata. To ensure it is available, run an update on the instance. Installing the updates isn't necessary.")
            color: ThemeColors.warning
            wrapMode: Text.WordWrap
            font.italic: true
        }
    }

    function updateResult() {
        if (!vm)
            return;
        var result = vm.generateModList(instanceId, {
            format: selectedFormat,
            template: customTemplate,
            includeVersion: includeVersion,
            includeAuthors: includeAuthors,
            includeUrl: includeUrl,
            includeFilename: includeFilename
        });

        resultText.text = result.plain || "";
        previewText.text = result.html || result.plain || "";
    }

    onOpened: {
        updateResult();
    }

    onAccepted: {
        if (vm) {
            vm.saveModList(instanceId, resultText.text, formatNames[selectedFormat]);
        }
    }
}
