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

Rectangle {
    id: placeholderPage
    color: ThemeColors.background

    property string pageName: "Unknown"

    ColumnLayout {
        anchors.centerIn: parent
        spacing: Theme.spacingL

        Image {
            Layout.alignment: Qt.AlignHCenter
            source: Theme.icon("bug")
            width: 64
            height: 64
            fillMode: Image.PreserveAspectFit
            opacity: 0.5
        }

        Label {
            Layout.alignment: Qt.AlignHCenter
            text: pageName
            font.pointSize: 16
            font.bold: true
            color: ThemeColors.text
        }

        Label {
            Layout.alignment: Qt.AlignHCenter
            text: qsTr("This page is not yet implemented in QML.\nPlease use the widget-based launcher for this feature.")
            color: ThemeColors.textSecondary
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
        }

        // Widget UI fallback removed - QML is the primary UI
    }
}
