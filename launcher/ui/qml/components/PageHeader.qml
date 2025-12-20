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

Item {
    id: root
    property string title: ""
    property string subtitle: ""
    implicitWidth: headerLayout.implicitWidth
    implicitHeight: headerLayout.implicitHeight

    RowLayout {
        id: headerLayout
        anchors.fill: parent
        spacing: 6
        Layout.fillWidth: true

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 2
            Label {
                text: root.title
                color: ThemeColors.text
                font.pixelSize: 18
                font.bold: true
                wrapMode: Text.WordWrap
            }
            Label {
                visible: root.subtitle.length > 0
                text: root.subtitle
                color: ThemeColors.textSecondary
                font.pixelSize: 12
                wrapMode: Text.WordWrap
            }
        }
    }
}
