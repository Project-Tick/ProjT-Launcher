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
    id: importPage
    color: ThemeColors.background

    property string importUrl: ""
    property var vm: ProjT.instancesVM

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingM
        spacing: Theme.spacingM

        // Header
        Label {
            text: qsTr("Import from zip")
            font.pointSize: 14
            font.bold: true
            color: ThemeColors.text
        }

        Label {
            text: qsTr("Import an instance from a zip file or URL.\nSupported formats: MultiMC, Prism Launcher, CurseForge, Modrinth, FTB")
            color: ThemeColors.textSecondary
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }

        // Import source
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Import Source")

            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingM

                // Local file
                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingS

                    RadioButton {
                        id: localFileRadio
                        text: qsTr("Local file")
                        checked: true
                    }

                    TextField {
                        id: localFileField
                        Layout.fillWidth: true
                        enabled: localFileRadio.checked
                        placeholderText: qsTr("Path to zip file...")
                        selectByMouse: true
                    }

                    Button {
                        text: qsTr("Browse...")
                        enabled: localFileRadio.checked
                        onClicked: {
                            if (ProjT.launcherVM) {
                                var path = ProjT.launcherVM.browseForFile(qsTr("Select modpack"), qsTr("Zip files (*.zip);;Modrinth packs (*.mrpack);;All files (*)"));
                                if (path.length > 0) {
                                    localFileField.text = path;
                                    importPage.importUrl = "file://" + path;
                                }
                            }
                        }
                    }
                }

                // URL
                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingS

                    RadioButton {
                        id: urlRadio
                        text: qsTr("URL")
                    }

                    TextField {
                        id: urlField
                        Layout.fillWidth: true
                        enabled: urlRadio.checked
                        placeholderText: qsTr("https://example.com/modpack.zip")
                        selectByMouse: true
                        onTextChanged: {
                            if (urlRadio.checked) {
                                importPage.importUrl = text;
                            }
                        }
                    }
                }
            }
        }

        // Drag and drop zone
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: dropArea.containsDrag ? ThemeColors.accent + "30" : ThemeColors.surface
            border.color: dropArea.containsDrag ? ThemeColors.accent : ThemeColors.border
            border.width: dropArea.containsDrag ? 2 : 1
            radius: Theme.radiusM

            DropArea {
                id: dropArea
                anchors.fill: parent

                onDropped: function (drop) {
                    if (drop.hasUrls) {
                        var url = drop.urls[0];
                        if (url.toString().startsWith("file://")) {
                            localFileRadio.checked = true;
                            localFileField.text = url.toString().replace("file://", "");
                            importPage.importUrl = url.toString();
                        } else {
                            urlRadio.checked = true;
                            urlField.text = url.toString();
                            importPage.importUrl = url.toString();
                        }
                    }
                }
            }

            ColumnLayout {
                anchors.centerIn: parent
                spacing: Theme.spacingM

                Image {
                    Layout.alignment: Qt.AlignHCenter
                    source: Theme.icon("viewfolder")
                    width: 64
                    height: 64
                    fillMode: Image.PreserveAspectFit
                    opacity: 0.5
                }

                Label {
                    Layout.alignment: Qt.AlignHCenter
                    text: qsTr("Drag and drop a modpack file here")
                    color: ThemeColors.textSecondary
                    font.pointSize: 12
                }

                Label {
                    Layout.alignment: Qt.AlignHCenter
                    text: qsTr("or use the options above to select a file")
                    color: ThemeColors.textSecondary
                    font.pointSize: 10
                }
            }
        }

        // Status
        RowLayout {
            Layout.fillWidth: true
            visible: importPage.importUrl.length > 0

            Label {
                text: qsTr("Selected:")
                color: ThemeColors.textSecondary
            }

            Label {
                text: importPage.importUrl
                color: ThemeColors.text
                elide: Text.ElideMiddle
                Layout.fillWidth: true
            }
        }
    }
}
