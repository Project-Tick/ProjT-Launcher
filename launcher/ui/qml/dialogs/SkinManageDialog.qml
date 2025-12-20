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
import "../components"

Dialog {
    id: skinDialog
    title: qsTr("Skin Upload")
    modal: true
    width: 900
    height: 700

    property var vm: ProjT.accountsVM
    property string selectedSkin: ""
    property bool isSlimModel: false
    property string selectedCape: ""
    property bool previewElytra: false

    footer: DialogButtonBox {
        Button {
            text: qsTr("Open Folder")
            flat: true
            onClicked: {
                if (vm)
                    vm.openSkinsFolder();
            }
        }
        Button {
            text: qsTr("Reset Skin")
            flat: true
            onClicked: {
                if (vm)
                    vm.resetSkin();
            }
        }
        Button {
            text: qsTr("OK")
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
        }
        Button {
            text: qsTr("Cancel")
            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: Theme.spacingM

        // Left panel: Skin preview and options
        ColumnLayout {
            Layout.preferredWidth: 280
            spacing: Theme.spacingM

            // Skin preview
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 280
                color: ThemeColors.backgroundAlt
                border.color: ThemeColors.border
                radius: Theme.radiusS

                // 3D skin preview would go here
                // For now, just a placeholder
                Image {
                    anchors.centerIn: parent
                    width: 128
                    height: 256
                    source: selectedSkin.length > 0 ? "file:///" + selectedSkin : ""
                    fillMode: Image.PreserveAspectFit
                    visible: selectedSkin.length > 0
                }

                Label {
                    anchors.centerIn: parent
                    text: qsTr("No skin selected")
                    color: ThemeColors.textSecondary
                    visible: selectedSkin.length === 0
                }
            }

            // Model selection
            GroupBox {
                Layout.fillWidth: true
                title: qsTr("Model")

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

                RowLayout {
                    anchors.fill: parent
                    spacing: Theme.spacingM

                    RadioButton {
                        text: qsTr("Classic")
                        checked: !isSlimModel
                        onCheckedChanged: {
                            if (checked)
                                isSlimModel = false;
                        }
                    }

                    RadioButton {
                        text: qsTr("Slim")
                        checked: isSlimModel
                        onCheckedChanged: {
                            if (checked)
                                isSlimModel = true;
                        }
                    }
                }
            }

            // Cape selection
            GroupBox {
                Layout.fillWidth: true
                Layout.fillHeight: true
                title: qsTr("Cape")

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

                    CheckBox {
                        text: qsTr("Preview Elytra")
                        checked: previewElytra
                        onCheckedChanged: previewElytra = checked
                    }

                    ComboBox {
                        id: capeCombo
                        Layout.fillWidth: true
                        model: vm ? vm.capesList : ["None"]
                        onCurrentTextChanged: selectedCape = currentText
                    }

                    // Cape preview
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        color: ThemeColors.surface
                        radius: Theme.radiusS

                        Image {
                            anchors.centerIn: parent
                            width: 64
                            height: 128
                            source: selectedCape.length > 0 && selectedCape !== "None" ? "file:///" + selectedCape : ""
                            fillMode: Image.PreserveAspectFit
                            visible: selectedCape.length > 0 && selectedCape !== "None"
                        }

                        Label {
                            anchors.centerIn: parent
                            text: qsTr("No cape")
                            color: ThemeColors.textSecondary
                            visible: selectedCape.length === 0 || selectedCape === "None"
                        }
                    }
                }
            }
        }

        // Right panel: Skin list
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: Theme.spacingS

            // Skin grid
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: ThemeColors.backgroundAlt
                border.color: ThemeColors.border
                radius: Theme.radiusS

                GridView {
                    id: skinGrid
                    anchors.fill: parent
                    anchors.margins: Theme.spacingS
                    clip: true
                    cellWidth: 100
                    cellHeight: 120
                    model: vm ? vm.skinsList : []

                    delegate: Rectangle {
                        width: 96
                        height: 116
                        color: modelData.path === selectedSkin ? ThemeColors.accent : ThemeColors.surface
                        border.color: ThemeColors.border
                        radius: Theme.radiusS

                        MouseArea {
                            anchors.fill: parent
                            onClicked: selectedSkin = modelData.path || modelData
                            onDoubleClicked: {
                                selectedSkin = modelData.path || modelData;
                                skinDialog.accept();
                            }
                        }

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 4
                            spacing: 2

                            Image {
                                Layout.alignment: Qt.AlignHCenter
                                width: 64
                                height: 80
                                source: modelData.thumbnail || ""
                                fillMode: Image.PreserveAspectFit
                            }

                            Label {
                                Layout.fillWidth: true
                                text: modelData.name || "Skin"
                                color: modelData.path === selectedSkin ? "white" : ThemeColors.text
                                font.pixelSize: 10
                                horizontalAlignment: Text.AlignHCenter
                                elide: Text.ElideRight
                            }
                        }
                    }

                    ScrollBar.vertical: ScrollBar {}
                }
            }

            // Import buttons
            RowLayout {
                Layout.fillWidth: true
                spacing: Theme.spacingS

                TextField {
                    id: urlField
                    Layout.fillWidth: true
                    placeholderText: qsTr("URL or username")
                }

                ThemedButton {
                    text: qsTr("Import URL")
                    size: "small"
                    onClicked: {
                        if (vm && urlField.text.length > 0) {
                            vm.importSkinFromUrl(urlField.text);
                        }
                    }
                }

                ThemedButton {
                    text: qsTr("Import User")
                    size: "small"
                    onClicked: {
                        if (vm && urlField.text.length > 0) {
                            vm.importSkinFromUser(urlField.text);
                        }
                    }
                }

                ThemedButton {
                    text: qsTr("Import File")
                    primary: true
                    size: "small"
                    onClicked: {
                        if (vm)
                            vm.browseSkinFile();
                    }
                }
            }
        }
    }

    onAccepted: {
        if (vm) {
            vm.uploadSkin(selectedSkin, isSlimModel, selectedCape);
        }
    }

    Connections {
        target: vm
        ignoreUnknownSignals: true
        function onSkinFileSelected(path) {
            selectedSkin = path;
        }
    }
}
