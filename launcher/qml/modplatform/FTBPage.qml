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
import "../Theme.js" as Theme

Page {
    id: ftbPage
    title: qsTr("FTB")
    background: Rectangle {
        color: ThemeColors.background
    }

    property var vm: typeof ProjT !== "undefined" && ProjT ? ProjT.ftbVM : null

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingM
        spacing: Theme.spacingM

        // Header
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingM

            Rectangle {
                Layout.preferredWidth: 48
                Layout.preferredHeight: 48
                radius: 8
                color: ThemeColors.surface

                Label {
                    anchors.centerIn: parent
                    text: "FTB"
                    color: ThemeColors.error
                    font.bold: true
                    font.pointSize: 12
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 2

                Label {
                    text: qsTr("Feed The Beast")
                    color: ThemeColors.text
                    font.bold: true
                    font.pointSize: Theme.fontSizeMedium
                }

                Label {
                    text: vm ? vm.statusMessage : ""
                    color: ThemeColors.textSecondary
                    font.pointSize: Theme.fontSizeSmall
                }
            }
        }

        // Search
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingS

            TextField {
                id: searchField
                Layout.fillWidth: true
                placeholderText: qsTr("Search modpacks...")
                onAccepted: if (vm)
                    vm.search(text)
            }

            Button {
                text: qsTr("Search")
                onClicked: if (vm)
                    vm.search(searchField.text)
            }

            Button {
                text: qsTr("Refresh")
                onClicked: if (vm)
                    vm.refresh()
            }
        }

        // Content
        SplitView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            orientation: Qt.Horizontal

            Frame {
                SplitView.preferredWidth: parent.width * 0.5
                SplitView.minimumWidth: 250

                ListView {
                    id: packList
                    anchors.fill: parent
                    clip: true
                    model: vm ? vm.packsModel : null
                    currentIndex: vm ? vm.selectedPackIndex : -1

                    delegate: ItemDelegate {
                        width: packList.width
                        height: 70
                        highlighted: ListView.isCurrentItem
                        onClicked: if (vm)
                            vm.selectPack(index)

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 8
                            spacing: 8

                            Image {
                                Layout.preferredWidth: 50
                                Layout.preferredHeight: 50
                                source: model.iconUrl || ""
                                fillMode: Image.PreserveAspectFit

                                Rectangle {
                                    anchors.fill: parent
                                    visible: parent.status !== Image.Ready
                                    color: ThemeColors.backgroundAlt
                                    radius: 4

                                    Label {
                                        anchors.centerIn: parent
                                        text: "FTB"
                                        color: ThemeColors.accent
                                        font.bold: true
                                        font.pointSize: 12
                                    }
                                }
                            }

                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 2

                                Label {
                                    text: model.name || model.display || ""
                                    color: ThemeColors.text
                                    font.bold: true
                                    elide: Text.ElideRight
                                    Layout.fillWidth: true
                                }

                                Label {
                                    text: model.toolTip || ""
                                    color: ThemeColors.textSecondary
                                    font.pointSize: 10
                                    elide: Text.ElideRight
                                    Layout.fillWidth: true
                                }
                            }
                        }
                    }

                    ScrollBar.vertical: ScrollBar {}
                }

                BusyIndicator {
                    anchors.centerIn: parent
                    running: vm ? vm.isLoading : false
                    visible: running
                }
            }

            Frame {
                SplitView.fillWidth: true

                ColumnLayout {
                    anchors.fill: parent
                    spacing: Theme.spacingM
                    visible: vm && vm.selectedPack && (vm.selectedPack.name !== undefined) && vm.selectedPack.name !== ""

                    Label {
                        text: (vm && vm.selectedPack && vm.selectedPack.name !== undefined) ? vm.selectedPack.name : ""
                        color: ThemeColors.text
                        font.bold: true
                        font.pointSize: 16
                    }

                    Label {
                        text: (vm && vm.selectedPack && vm.selectedPack.author !== undefined) ? vm.selectedPack.author : ""
                        color: ThemeColors.textSecondary
                    }

                    ScrollView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        Label {
                            text: (vm && vm.selectedPack && vm.selectedPack.description !== undefined) ? vm.selectedPack.description : ""
                            wrapMode: Text.WordWrap
                            width: parent.width
                            color: ThemeColors.text
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        visible: vm && vm.selectedPackVersions && vm.selectedPackVersions.length > 0

                        Label {
                            text: qsTr("Version:")
                        }

                        ComboBox {
                            Layout.fillWidth: true
                            model: vm ? vm.selectedPackVersions : []
                            currentIndex: vm ? vm.selectedVersionIndex : 0
                            onActivated: if (vm)
                                vm.selectVersion(index)
                        }
                    }

                    Button {
                        Layout.fillWidth: true
                        text: qsTr("Install")
                        highlighted: true
                        enabled: vm && vm.selectedPackIndex >= 0
                        onClicked: if (vm)
                            vm.installSelected("", "")
                    }
                }

                Label {
                    anchors.centerIn: parent
                    text: qsTr("Select a pack")
                    color: ThemeColors.textSecondary
                    visible: !(vm && vm.selectedPack && vm.selectedPack.name)
                }
            }
        }
    }

    Connections {
        target: vm
        function onInstallFinished(success, message) {
            resultDialog.success = success;
            resultDialog.message = message;
            resultDialog.open();
        }
    }

    Dialog {
        id: resultDialog
        title: success ? qsTr("Success") : qsTr("Error")
        modal: true
        standardButtons: Dialog.Ok
        property bool success: false
        property string message: ""
        Label {
            text: resultDialog.message
            wrapMode: Text.WordWrap
        }
    }
}
