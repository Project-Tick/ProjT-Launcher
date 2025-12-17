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
    id: upperBar
    color: ThemeColors.toolBar
    height: 36

    // Properties - bound to ViewModels with safe defaults
    property string newsHeadline: ProjT.newsVM.latestHeadline || qsTr("Welcome to ProjT Launcher")
    property bool hasUpdate: ProjT.launcherVM.hasUpdate || false
    property string updateVersion: ProjT.launcherVM.updateVersion || ""

    // Signals
    signal moreNewsClicked
    signal updateClicked

    Rectangle {
        anchors.fill: parent
        color: ThemeColors.toolBar

        // Top border only
        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            height: 1
            color: ThemeColors.border
        }

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: Theme.spacingM
            anchors.rightMargin: Theme.spacingM
            spacing: Theme.spacingM

            // === News Label ===
            Label {
                text: qsTr("News:")
                color: ThemeColors.textSecondary
                font.pointSize: 10
                Layout.alignment: Qt.AlignVCenter
            }

            // === News Headline ===
            Label {
                id: headlineLabel
                text: newsHeadline
                color: ThemeColors.text
                font.pointSize: 10
                elide: Text.ElideRight
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: upperBar.moreNewsClicked()

                    hoverEnabled: true
                    onEntered: headlineLabel.color = ThemeColors.accent
                    onExited: headlineLabel.color = ThemeColors.text
                }
            }

            // === More News Button ===
            ThemedToolButton {
                text: qsTr("More News")
                Layout.preferredHeight: 28
                Layout.alignment: Qt.AlignVCenter
                customTextColor: ThemeColors.accent

                onClicked: upperBar.moreNewsClicked()
            }

            // === Separator (visible only when update is available) ===
            Rectangle {
                visible: hasUpdate
                Layout.preferredWidth: 1
                Layout.preferredHeight: 20
                Layout.alignment: Qt.AlignVCenter
                color: ThemeColors.border
            }

            // === Update Available Badge ===
            Rectangle {
                visible: hasUpdate
                Layout.preferredWidth: updateRow.implicitWidth + 16
                Layout.preferredHeight: 28
                Layout.alignment: Qt.AlignVCenter
                radius: 14
                color: Qt.darker(ThemeColors.info, 1.5)
                border.color: ThemeColors.info
                border.width: 1

                RowLayout {
                    id: updateRow
                    anchors.centerIn: parent
                    spacing: 4

                    Text {
                        text: "⬆"
                        font.pointSize: 10
                        color: ThemeColors.info
                    }

                    Label {
                        text: qsTr("Update: %1").arg(updateVersion)
                        color: Qt.lighter(ThemeColors.info, 1.3)
                        font.pointSize: 9
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: upperBar.updateClicked()
                }
            }
        }
    }
}
