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
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

Rectangle {
    id: root
    width: 280
    height: 100
    color: "#00000000"
    border.color: "#40FFFFFF"
    radius: 4

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 4

        Text {
            text: launcherViewModel ? launcherViewModel.displayName : ""
            font.bold: true
            color: "white"
            wrapMode: Text.WordWrap
        }

        Text {
            text: launcherViewModel ? launcherViewModel.versionString : ""
            color: "#CCCCCC"
            wrapMode: Text.WordWrap
        }

        Text {
            text: launcherViewModel && launcherViewModel.busy ? qsTr("Status: Busy") : qsTr("Status: Idle")
            color: launcherViewModel && launcherViewModel.busy ? "#FFB74D" : "#A5D6A7"
            font.pointSize: 10
        }
    }
}
