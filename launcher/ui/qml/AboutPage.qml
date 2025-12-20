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
import "components"

Rectangle {
    objectName: "about"
    color: ThemeColors.background
    width: parent ? parent.width : 640
    height: parent ? parent.height : 480

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        PageHeader {
            Layout.fillWidth: true
            title: ProjT.launcherVM ? ProjT.launcherVM.displayName + " " + ProjT.launcherVM.versionString : qsTr("About")
            subtitle: ProjT.launcherVM ? ProjT.launcherVM.gitRef + " (" + ProjT.launcherVM.gitCommit + ")" : ""
        }

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Text {
                width: parent ? parent.width : implicitWidth
                wrapMode: Text.WordWrap
                textFormat: Text.RichText
                color: ThemeColors.text
                text: ProjT.launcherVM ? ProjT.launcherVM.aboutHtml : ""
            }
        }
    }
}
