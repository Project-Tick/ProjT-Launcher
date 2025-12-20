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

ScrollView {
    id: iconThemeGallery
    clip: true

    property var themeVM: ProjT.themeVM
    contentWidth: Math.max(0, grid.width + Theme.spacingM * 2)
    contentHeight: grid.implicitHeight + Theme.spacingM * 2

    function iconSource(themeId, name) {
        if (!themeId) {
            return Theme.icon(name);
        }
        return "qrc:/icons/" + themeId + "/scalable/" + name + ".svg";
    }

    GridLayout {
        id: grid
        width: Math.max(0, iconThemeGallery.availableWidth - Theme.spacingM * 2)
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: Theme.spacingM
        anchors.topMargin: Theme.spacingM
        columns: Math.max(1, Math.floor(width / 220))
        columnSpacing: Theme.spacingM
        rowSpacing: Theme.spacingM

        Repeater {
            model: themeVM ? themeVM.availableIconThemes : null

            delegate: Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 140
                color: ThemeColors.surface
                border.color: isCurrentTheme ? ThemeColors.highlight : ThemeColors.border
                border.width: isCurrentTheme ? 2 : 1
                radius: Theme.radiusM

                property string themeId: model.themeId
                property bool isCurrentTheme: themeVM && model.themeId === themeVM.currentIconTheme

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: Theme.spacingM
                    spacing: Theme.spacingS

                    // Icon Theme Name
                    Label {
                        text: model.name || ""
                        font.pixelSize: Theme.fontSubtitle
                        font.bold: true
                        color: isCurrentTheme ? ThemeColors.highlight : ThemeColors.text
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                    }

                    // Icon Preview (if path available)
                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        RowLayout {
                            anchors.centerIn: parent
                            spacing: Theme.spacingS

                            Repeater {
                                model: ["settings", "accounts", "news", "minecraft"]
                                delegate: Image {
                                    width: 20
                                    height: 20
                                    fillMode: Image.PreserveAspectFit
                                    source: iconThemeGallery.iconSource(themeId, modelData)
                                    onStatusChanged: {
                                        if (status === Image.Error) {
                                            source = Theme.icon(modelData);
                                        }
                                    }
                                }
                            }
                        }
                    }

                    // Apply Button
                    ThemedButton {
                        Layout.fillWidth: true
                        text: isCurrentTheme ? qsTr("Current") : qsTr("Apply")
                        enabled: !isCurrentTheme
                        size: "small"
                        primary: !isCurrentTheme

                        onClicked: {
                            if (themeVM) {
                                themeVM.setCurrentIconTheme(model.themeId);
                            }
                        }
                    }
                }

                // Current theme indicator
                Rectangle {
                    visible: isCurrentTheme
                    anchors.top: parent.top
                    anchors.right: parent.right
                    anchors.margins: Theme.spacingS
                    width: 24
                    height: 24
                    radius: 12
                    color: ThemeColors.highlight

                    Label {
                        anchors.centerIn: parent
                        text: "✓"
                        color: ThemeColors.highlightedText
                        font.bold: true
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: isCurrentTheme ? Qt.ArrowCursor : Qt.PointingHandCursor
                    enabled: !isCurrentTheme
                    onClicked: {
                        if (themeVM) {
                            themeVM.setCurrentIconTheme(model.themeId);
                        }
                    }
                }
            }
        }

        // Empty state
        Label {
            visible: !themeVM || (themeVM.availableIconThemes && themeVM.availableIconThemes.rowCount() === 0)
            text: qsTr("No icon themes available")
            color: ThemeColors.textSecondary
            font.italic: true
            Layout.columnSpan: parent.columns
            Layout.alignment: Qt.AlignHCenter
        }
    }
}
