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

Frame {
    id: infoFrame

    property string iconSource: ""
    property string name: ""
    property string description: ""
    property string license: ""
    property string issueTrackerUrl: ""

    implicitHeight: contentColumn.implicitHeight + topPadding + bottomPadding

    background: Rectangle {
        color: ThemeColors.surface
        border.color: ThemeColors.border
        radius: Theme.radiusS
    }

    RowLayout {
        anchors.fill: parent
        spacing: Theme.spacingM

        // Icon
        Image {
            id: iconImage
            Layout.preferredWidth: 64
            Layout.preferredHeight: 64
            Layout.alignment: Qt.AlignTop
            source: iconSource
            fillMode: Image.PreserveAspectFit
            visible: iconSource.length > 0

            // Fallback placeholder
            Rectangle {
                anchors.fill: parent
                color: ThemeColors.backgroundAlt
                visible: iconImage.status !== Image.Ready && iconSource.length > 0

                Label {
                    anchors.centerIn: parent
                    text: "?"
                    font.pixelSize: 24
                    color: ThemeColors.textSecondary
                }
            }
        }

        // Content
        ColumnLayout {
            id: contentColumn
            Layout.fillWidth: true
            spacing: Theme.spacingXS

            // Name
            Label {
                id: nameLabel
                Layout.fillWidth: true
                text: name
                font.bold: true
                font.pixelSize: 14
                color: ThemeColors.text
                wrapMode: Text.WordWrap
                visible: name.length > 0
                textFormat: Text.RichText
                onLinkActivated: function (link) {
                    Qt.openUrlExternally(link);
                }

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.NoButton
                    cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                }
            }

            // Description
            Label {
                id: descriptionLabel
                Layout.fillWidth: true
                text: description
                color: ThemeColors.textSecondary
                wrapMode: Text.WordWrap
                visible: description.length > 0
                textFormat: Text.RichText
                onLinkActivated: function (link) {
                    Qt.openUrlExternally(link);
                }

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.NoButton
                    cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                }
            }

            // License
            Label {
                id: licenseLabel
                Layout.fillWidth: true
                text: license
                color: ThemeColors.textSecondary
                wrapMode: Text.WordWrap
                visible: license.length > 0
                textFormat: Text.RichText
                onLinkActivated: function (link) {
                    Qt.openUrlExternally(link);
                }

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.NoButton
                    cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                }
            }

            // Issue tracker
            Label {
                id: issueTrackerLabel
                Layout.fillWidth: true
                text: issueTrackerUrl.length > 0 ? qsTr("<a href=\"%1\">Issue Tracker</a>").arg(issueTrackerUrl) : ""
                color: ThemeColors.textSecondary
                wrapMode: Text.WordWrap
                visible: issueTrackerUrl.length > 0
                textFormat: Text.RichText
                onLinkActivated: function (link) {
                    Qt.openUrlExternally(link);
                }

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.NoButton
                    cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                }
            }
        }
    }
}
