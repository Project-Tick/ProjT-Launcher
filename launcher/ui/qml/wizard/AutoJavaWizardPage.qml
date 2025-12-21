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
    id: autoJavaPage
    color: ThemeColors.background

    property bool autoDownloadEnabled: true

    signal settingsChanged

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingM
        spacing: Theme.spacingS

        // Title
        Label {
            text: qsTr("New Feature Alert!")
            font.pointSize: 14
            font.bold: true
            color: ThemeColors.text
        }

        // Description
        Label {
            Layout.fillWidth: true
            text: qsTr("We've added a feature to automatically download the correct Java version for each version of Minecraft (this can be changed in the Java Settings). Would you like to enable or disable this feature?")
            color: ThemeColors.text
            wrapMode: Text.WordWrap
            font.pixelSize: Theme.fontBody
        }

        // Separator
        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: ThemeColors.border
        }

        // Options
        ColumnLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingS

            RadioButton {
                id: enableRadio
                text: qsTr("Enable Auto-Download")
                checked: autoDownloadEnabled
                onCheckedChanged: {
                    if (checked) {
                        autoDownloadEnabled = true;
                        settingsChanged();
                    }
                }
            }

            RadioButton {
                text: qsTr("Disable Auto-Download")
                checked: !autoDownloadEnabled
                onCheckedChanged: {
                    if (checked) {
                        autoDownloadEnabled = false;
                        settingsChanged();
                    }
                }
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
