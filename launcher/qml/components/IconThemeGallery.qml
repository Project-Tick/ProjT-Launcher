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

ScrollView {
    id: iconThemeGallery
    clip: true

    property var themeVM: ProjT.themeVM

    GridLayout {
        width: iconThemeGallery.width - Theme.spacingL
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

                        Label {
                            anchors.centerIn: parent
                            text: qsTr("Icon Theme")
                            color: ThemeColors.textSecondary
                            font.pixelSize: Theme.fontCaption
                            font.italic: true
                        }
                    }

                    // Apply Button
                    Button {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 32
                        text: isCurrentTheme ? qsTr("Current") : qsTr("Apply")
                        enabled: !isCurrentTheme
                        flat: false

                        onClicked: {
                            if (themeVM) {
                                themeVM.setCurrentIconTheme(model.themeId);
                            }
                        }

                        background: Rectangle {
                            implicitHeight: 32
                            color: {
                                if (!parent.enabled) {
                                    return Qt.rgba(ThemeColors.button.r, ThemeColors.button.g, ThemeColors.button.b, 0.5);
                                }
                                if (parent.down) {
                                    return Qt.darker(isCurrentTheme ? ThemeColors.highlight : ThemeColors.button, 1.2);
                                }
                                if (parent.hovered) {
                                    return Qt.lighter(isCurrentTheme ? ThemeColors.highlight : ThemeColors.button, 1.1);
                                }
                                return isCurrentTheme ? ThemeColors.highlight : ThemeColors.button;
                            }
                            radius: 4
                            border.width: parent.visualFocus ? 2 : 0
                            border.color: ThemeColors.highlight

                            Behavior on color {
                                ColorAnimation {
                                    duration: 100
                                }
                            }
                        }

                        contentItem: Text {
                            text: parent.text
                            color: {
                                if (!parent.enabled) {
                                    return Qt.rgba(ThemeColors.buttonText.r, ThemeColors.buttonText.g, ThemeColors.buttonText.b, 0.5);
                                }
                                return isCurrentTheme ? ThemeColors.highlightedText : ThemeColors.buttonText;
                            }
                            font.pixelSize: 13
                            font.bold: isCurrentTheme
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
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
