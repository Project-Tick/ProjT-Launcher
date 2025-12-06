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
import ProjTLauncher 1.0

Rectangle {
    id: themePreview
    width: parent ? parent.width : 300
    height: 200
    color: ThemeColors.window
    border.color: ThemeColors.border
    border.width: 1
    radius: ThemeColors.radiusM

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: ThemeColors.spacingM
        spacing: ThemeColors.spacingS

        Label {
            text: qsTr("Theme Preview")
            font.pixelSize: ThemeColors.fontHeader
            font.bold: true
            color: ThemeColors.windowText
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            color: ThemeColors.base
            border.color: ThemeColors.border
            radius: ThemeColors.radiusS

            Label {
                anchors.centerIn: parent
                text: qsTr("Base Background")
                color: ThemeColors.text
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            color: ThemeColors.alternateBase
            border.color: ThemeColors.border
            radius: ThemeColors.radiusS

            Label {
                anchors.centerIn: parent
                text: qsTr("Alternate Base")
                color: ThemeColors.text
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: ThemeColors.spacingS

            Button {
                Layout.fillWidth: true
                text: qsTr("Button")
                background: Rectangle {
                    color: parent.pressed ? ThemeColors.pressed : (parent.hovered ? ThemeColors.hover : ThemeColors.button)
                    radius: ThemeColors.radiusS
                    border.color: ThemeColors.border
                }
                contentItem: Text {
                    text: parent.text
                    color: ThemeColors.buttonText
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }

            Button {
                Layout.fillWidth: true
                text: qsTr("Highlight")
                background: Rectangle {
                    color: parent.pressed ? Qt.darker(ThemeColors.highlight, 1.2) : (parent.hovered ? Qt.lighter(ThemeColors.highlight, 1.1) : ThemeColors.highlight)
                    radius: ThemeColors.radiusS
                }
                contentItem: Text {
                    text: parent.text
                    color: ThemeColors.highlightedText
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: ThemeColors.spacingS

            Label {
                text: qsTr("Link:")
                color: ThemeColors.textSecondary
            }

            Label {
                text: qsTr("Click here")
                color: ThemeColors.link
                font.underline: true

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    hoverEnabled: true
                    onEntered: parent.color = Qt.lighter(ThemeColors.link, 1.2)
                    onExited: parent.color = ThemeColors.link
                }
            }
        }
    }
}
