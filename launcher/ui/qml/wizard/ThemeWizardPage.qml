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
import "../Theme.js" as Theme

Rectangle {
    id: themePage
    color: ThemeColors.background

    property var vm: ProjT.themeVM
    property string selectedTheme: "system"

    signal themeChanged(string themeId)

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingL
        spacing: Theme.spacingM

        // Title
        Label {
            text: qsTr("Choose Your Theme")
            font.pixelSize: 18
            font.bold: true
            color: ThemeColors.text
        }

        // Description
        Label {
            Layout.fillWidth: true
            text: qsTr("Select a theme for the launcher. You can change this later in Settings.")
            color: ThemeColors.text
            wrapMode: Text.WordWrap
        }

        // Separator
        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: ThemeColors.border
        }

        // Theme options
        GridLayout {
            Layout.fillWidth: true
            columns: 3
            rowSpacing: Theme.spacingM
            columnSpacing: Theme.spacingM

            // System Theme
            ThemeOption {
                themeId: "system"
                themeName: qsTr("System")
                themeDescription: qsTr("Follow system theme")
                previewColors: ["#f5f5f5", "#333333"]
                selected: selectedTheme === "system"
                onClicked: {
                    selectedTheme = "system";
                    themeChanged("system");
                }
            }

            // Light Theme
            ThemeOption {
                themeId: "light"
                themeName: qsTr("Light")
                themeDescription: qsTr("Light background")
                previewColors: ["#ffffff", "#1a1a1a"]
                selected: selectedTheme === "light"
                onClicked: {
                    selectedTheme = "light";
                    themeChanged("light");
                }
            }

            // Dark Theme
            ThemeOption {
                themeId: "dark"
                themeName: qsTr("Dark")
                themeDescription: qsTr("Dark background")
                previewColors: ["#1e1e1e", "#ffffff"]
                selected: selectedTheme === "dark"
                onClicked: {
                    selectedTheme = "dark";
                    themeChanged("dark");
                }
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }

    // Theme preview component
    component ThemeOption: Rectangle {
        property string themeId
        property string themeName
        property string themeDescription
        property var previewColors: ["#ffffff", "#000000"]
        property bool selected: false

        signal clicked

        Layout.fillWidth: true
        Layout.preferredHeight: 120
        radius: Theme.radiusM
        border.color: selected ? ThemeColors.accent : ThemeColors.border
        border.width: selected ? 2 : 1
        color: ThemeColors.surface

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: parent.clicked()
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: Theme.spacingM
            spacing: Theme.spacingS

            // Preview
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 50
                radius: Theme.radiusS
                color: previewColors[0]

                RowLayout {
                    anchors.centerIn: parent
                    spacing: Theme.spacingS

                    Rectangle {
                        width: 30
                        height: 30
                        radius: Theme.radiusS
                        color: previewColors[1]
                    }

                    Rectangle {
                        width: 60
                        height: 8
                        radius: 4
                        color: previewColors[1]
                        opacity: 0.7
                    }
                }
            }

            // Name
            Label {
                text: themeName
                font.bold: true
                color: ThemeColors.text
            }

            // Description
            Label {
                text: themeDescription
                font.pixelSize: 11
                color: ThemeColors.textSecondary
            }
        }

        // Selected indicator
        Rectangle {
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: Theme.spacingS
            width: 24
            height: 24
            radius: 12
            color: selected ? ThemeColors.accent : "transparent"
            border.color: selected ? ThemeColors.accent : ThemeColors.border
            visible: selected

            Label {
                anchors.centerIn: parent
                text: "✓"
                color: "white"
                font.bold: true
            }
        }
    }
}
