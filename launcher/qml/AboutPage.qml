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
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "components"

Rectangle {
    objectName: "about"
    color: "#1b1b1b"
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
                color: "#cfd8dc"
                text: ProjT.launcherVM ? ProjT.launcherVM.aboutHtml : ""
            }
        }
    }
}
