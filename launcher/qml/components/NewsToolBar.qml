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
    id: newsToolbar
    height: 32

    // Theme binding for reactive updates
    property var themeVM: ProjT.themeVM
    property int _themeUpdateCount: 0

    color: {
        var _ = _themeUpdateCount;
        return themeVM ? Qt.darker(themeVM.windowColor, 1.05) : ThemeColors.toolBar;
    }

    property color toolBarColor: {
        var _ = _themeUpdateCount;
        return themeVM ? Qt.darker(themeVM.windowColor, 1.05) : ThemeColors.toolBar;
    }
    property color borderColor: {
        var _ = _themeUpdateCount;
        return themeVM ? Qt.darker(themeVM.windowColor, 1.2) : ThemeColors.border;
    }
    property color textColor: {
        var _ = _themeUpdateCount;
        return themeVM ? themeVM.textColor : ThemeColors.text;
    }
    property color textSecondaryColor: {
        var _ = _themeUpdateCount;
        return themeVM ? Qt.darker(themeVM.textColor, 1.3) : ThemeColors.textSecondary;
    }
    property color accentColor: {
        var _ = _themeUpdateCount;
        return themeVM ? themeVM.highlightColor : ThemeColors.accent;
    }

    Connections {
        target: themeVM
        function onThemeColorsChanged() {
            newsToolbar._themeUpdateCount++;
        }
    }

    // News data from ViewModel
    readonly property var newsVM: ProjT.newsVM
    readonly property string latestHeadline: newsVM ? newsVM.latestHeadline : ""
    readonly property bool isBusy: newsVM ? newsVM.busy : false

    // Signals
    signal newsClicked
    signal moreNewsClicked

    Rectangle {
        anchors.fill: parent
        color: newsToolbar.toolBarColor

        // Top border only
        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            height: 1
            color: newsToolbar.borderColor
        }

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: Theme.spacingS
            anchors.rightMargin: Theme.spacingS
            spacing: Theme.spacingS

            // === News Icon ===
            Label {
                text: "📰"
                font.pointSize: 12
                Layout.alignment: Qt.AlignVCenter
            }

            // News label
            Label {
                text: qsTr("News:")
                color: newsToolbar.textSecondaryColor
                font.pointSize: 10
                Layout.alignment: Qt.AlignVCenter
            }

            // === News Headline (clickable) ===
            Label {
                id: headlineLabel
                text: isBusy ? qsTr("Loading news...") : (latestHeadline.length > 0 ? latestHeadline : qsTr("No news available"))
                color: newsToolbar.textColor
                font.pointSize: 10
                elide: Text.ElideRight
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter

                property bool isHovered: false

                Behavior on color {
                    ColorAnimation {
                        duration: 100
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    hoverEnabled: true

                    onClicked: newsToolbar.newsClicked()
                    onEntered: headlineLabel.color = newsToolbar.accentColor
                    onExited: headlineLabel.color = newsToolbar.textColor
                }
            }

            // === More News Button ===
            ThemedToolButton {
                text: qsTr("More News...")
                icon.name: "go-next"
                display: AbstractButton.TextBesideIcon
                Layout.preferredHeight: 26

                onClicked: newsToolbar.moreNewsClicked()

                ToolTip.text: qsTr("Open the development blog to read more news")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }
        }
    }
}
