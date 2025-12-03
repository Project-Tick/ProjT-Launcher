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
