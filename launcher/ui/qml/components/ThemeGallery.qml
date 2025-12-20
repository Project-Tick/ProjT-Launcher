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
import "../Theme.js" as Theme

ScrollView {
    id: themeGallery
    clip: true

    property var themeVM: ProjT.themeVM

    GridLayout {
        width: themeGallery.width - Theme.spacingL
        columns: Math.max(1, Math.floor(width / 280))
        columnSpacing: Theme.spacingM
        rowSpacing: Theme.spacingM

        Repeater {
            model: themeVM ? themeVM.availableThemes : null

            delegate: Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 180
                color: ThemeColors.surface
                border.color: isCurrentTheme ? ThemeColors.highlight : ThemeColors.border
                border.width: isCurrentTheme ? 2 : 1
                radius: Theme.radiusM

                property bool isCurrentTheme: themeVM && model.themeId === themeVM.currentTheme

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: Theme.spacingM
                    spacing: Theme.spacingS

                    // Theme Name
                    Label {
                        text: model.name || ""
                        font.pixelSize: Theme.fontSubtitle
                        font.bold: true
                        color: isCurrentTheme ? ThemeColors.highlight : ThemeColors.text
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                    }

                    // Theme Description
                    Label {
                        text: model.tooltip || ""
                        font.pixelSize: Theme.fontCaption
                        color: ThemeColors.textSecondary
                        wrapMode: Text.WordWrap
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        visible: text.length > 0
                    }

                    // Platform compatibility badge
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 20
                        color: "transparent"
                        visible: model.tooltip && (model.tooltip.indexOf("Windows") >= 0 || model.tooltip.indexOf("macOS") >= 0 || model.tooltip.indexOf("Linux") >= 0)

                        RowLayout {
                            anchors.fill: parent
                            spacing: 4

                            Label {
                                text: {
                                    var platforms = [];
                                    if (model.tooltip) {
                                        if (model.tooltip.indexOf("Windows") >= 0)
                                            platforms.push("Win");
                                        if (model.tooltip.indexOf("macOS") >= 0 || model.tooltip.indexOf("Mac") >= 0)
                                            platforms.push("Mac");
                                        if (model.tooltip.indexOf("Linux") >= 0)
                                            platforms.push("Linux");
                                    }
                                    return platforms.length > 0 ? "🖥️ " + platforms.join(", ") : "";
                                }
                                font.pixelSize: 9
                                color: ThemeColors.textSecondary
                                visible: text.length > 0
                            }
                        }
                    }

                    // Color Preview Strip
                    RowLayout {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 30
                        spacing: 2

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            color: ThemeColors.window
                            radius: 2
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            color: ThemeColors.base
                            radius: 2
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            color: ThemeColors.button
                            radius: 2
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            color: ThemeColors.highlight
                            radius: 2
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
                                themeVM.setCurrentTheme(model.themeId);
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
                            themeVM.setCurrentTheme(model.themeId);
                        }
                    }
                }
            }
        }

        // Empty state
        Label {
            visible: !themeVM || (themeVM.availableThemes && themeVM.availableThemes.rowCount() === 0)
            text: qsTr("No themes available")
            color: ThemeColors.textSecondary
            font.italic: true
            Layout.columnSpan: parent.columns
            Layout.alignment: Qt.AlignHCenter
        }
    }
}
