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
    id: pastePage
    color: ThemeColors.background

    property bool useDefaultService: true

    signal settingsChanged

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingL
        spacing: Theme.spacingM

        // Description
        Label {
            Layout.fillWidth: true
            text: qsTr("The default paste service has changed to mclo.gs, please choose what you want to do with your settings.")
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
                text: qsTr("Use new default service")
                checked: useDefaultService
                onCheckedChanged: {
                    if (checked) {
                        useDefaultService = true;
                        settingsChanged();
                    }
                }
            }

            RadioButton {
                text: qsTr("Keep previous settings")
                checked: !useDefaultService
                onCheckedChanged: {
                    if (checked) {
                        useDefaultService = false;
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
