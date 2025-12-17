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

Rectangle {
    id: importPage
    color: ThemeColors.background

    property var vm: ProjT.instancesVM
    property string modpackPath: ""

    signal importRequested(string path)

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingL
        spacing: Theme.spacingM

        // Input section
        Label {
            text: qsTr("Local file or link to a direct download:")
            color: ThemeColors.text
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingS

            TextField {
                id: modpackEdit
                Layout.fillWidth: true
                placeholderText: "http://"
                text: modpackPath
                onTextChanged: modpackPath = text
            }

            Button {
                text: qsTr("Browse")
                onClicked: {
                    if (vm)
                        vm.browseModpack();
                }
            }
        }

        // Supported formats
        ColumnLayout {
            Layout.fillWidth: true
            Layout.topMargin: Theme.spacingL
            spacing: Theme.spacingS

            Label {
                Layout.fillWidth: true
                text: qsTr("The following file types are implemented (both for local files and URLs):")
                color: ThemeColors.text
                horizontalAlignment: Text.AlignHCenter
            }

            Label {
                Layout.fillWidth: true
                text: qsTr("- CurseForge modpacks (ZIP / curseforge:// URL)")
                color: ThemeColors.textSecondary
                horizontalAlignment: Text.AlignHCenter
            }

            Label {
                Layout.fillWidth: true
                text: qsTr("- Modrinth modpacks (ZIP and mrpack)")
                color: ThemeColors.textSecondary
                horizontalAlignment: Text.AlignHCenter
            }

            Label {
                Layout.fillWidth: true
                text: qsTr("- ProjT Launcher, Prism Launcher, PolyMC or MultiMC exported instances (ZIP)")
                color: ThemeColors.textSecondary
                horizontalAlignment: Text.AlignHCenter
            }

            Label {
                Layout.fillWidth: true
                text: qsTr("- Technic modpacks (ZIP)")
                color: ThemeColors.textSecondary
                horizontalAlignment: Text.AlignHCenter
            }
        }

        Item {
            Layout.fillHeight: true
        }

        // Import button
        Button {
            Layout.alignment: Qt.AlignHCenter
            text: qsTr("Import")
            enabled: modpackPath.length > 0
            highlighted: true
            onClicked: {
                importRequested(modpackPath);
                if (vm)
                    vm.importModpack(modpackPath);
            }
        }
    }

    Connections {
        target: vm
        ignoreUnknownSignals: true
        function onModpackSelected(path) {
            modpackEdit.text = path;
        }
    }
}
