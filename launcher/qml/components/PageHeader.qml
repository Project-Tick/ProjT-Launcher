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
                color: "#eceff1"
                font.pixelSize: 18
                font.bold: true
                wrapMode: Text.WordWrap
            }
            Label {
                visible: root.subtitle.length > 0
                text: root.subtitle
                color: "#b0bec5"
                font.pixelSize: 12
                wrapMode: Text.WordWrap
            }
        }
    }
}
