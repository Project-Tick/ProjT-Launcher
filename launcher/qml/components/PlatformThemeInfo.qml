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

Rectangle {
    id: platformInfo
    color: ThemeColors.surface
    border.color: ThemeColors.border
    radius: Theme.radiusM

    property var themeVM: ProjT.themeVM

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingM
        spacing: Theme.spacingS

        // Header
        Label {
            text: qsTr("Platform & Theme Information")
            font.pixelSize: Theme.fontSubtitle
            font.bold: true
            color: ThemeColors.text
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: ThemeColors.border
        }

        // Current Platform
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingM

            Label {
                text: "🖥️"
                font.pixelSize: 16
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 2

                Label {
                    text: qsTr("Current Platform")
                    font.pixelSize: Theme.fontCaption
                    color: ThemeColors.textSecondary
                }

                Label {
                    text: {
                        if (Qt.platform.os === "windows")
                            return "Windows";
                        if (Qt.platform.os === "osx")
                            return "macOS";
                        if (Qt.platform.os === "linux")
                            return "Linux";
                        return Qt.platform.os;
                    }
                    font.pixelSize: Theme.fontBody
                    font.bold: true
                    color: ThemeColors.text
                }
            }
        }

        // Platform-specific features
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Platform Features")

            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS

                // Windows
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: windowsInfo.implicitHeight + Theme.spacingS * 2
                    color: Qt.platform.os === "windows" ? Qt.rgba(ThemeColors.highlight.r, ThemeColors.highlight.g, ThemeColors.highlight.b, 0.1) : "transparent"
                    border.color: Qt.platform.os === "windows" ? ThemeColors.highlight : ThemeColors.border
                    radius: Theme.radiusS

                    ColumnLayout {
                        id: windowsInfo
                        anchors.fill: parent
                        anchors.margins: Theme.spacingS
                        spacing: 4

                        Label {
                            text: "🪟 Windows"
                            font.bold: true
                            color: ThemeColors.text
                        }

                        Label {
                            text: qsTr("• Windows 11 native theme (Win11 only)\n• Fusion theme\n• Windows Vista style\n• Custom QSS/CSS themes")
                            font.pixelSize: Theme.fontCaption
                            color: ThemeColors.textSecondary
                            Layout.fillWidth: true
                            wrapMode: Text.WordWrap
                        }
                    }
                }

                // macOS
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: macosInfo.implicitHeight + Theme.spacingS * 2
                    color: Qt.platform.os === "osx" ? Qt.rgba(ThemeColors.highlight.r, ThemeColors.highlight.g, ThemeColors.highlight.b, 0.1) : "transparent"
                    border.color: Qt.platform.os === "osx" ? ThemeColors.highlight : ThemeColors.border
                    radius: Theme.radiusS

                    ColumnLayout {
                        id: macosInfo
                        anchors.fill: parent
                        anchors.margins: Theme.spacingS
                        spacing: 4

                        Label {
                            text: "🍎 macOS"
                            font.bold: true
                            color: ThemeColors.text
                        }

                        Label {
                            text: qsTr("• Native Aqua appearance\n• Dark Aqua mode support\n• Dynamic titlebar colors\n• System palette integration")
                            font.pixelSize: Theme.fontCaption
                            color: ThemeColors.textSecondary
                            Layout.fillWidth: true
                            wrapMode: Text.WordWrap
                        }
                    }
                }

                // Linux
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: linuxInfo.implicitHeight + Theme.spacingS * 2
                    color: Qt.platform.os === "linux" ? Qt.rgba(ThemeColors.highlight.r, ThemeColors.highlight.g, ThemeColors.highlight.b, 0.1) : "transparent"
                    border.color: Qt.platform.os === "linux" ? ThemeColors.highlight : ThemeColors.border
                    radius: Theme.radiusS

                    ColumnLayout {
                        id: linuxInfo
                        anchors.fill: parent
                        anchors.margins: Theme.spacingS
                        spacing: 4

                        Label {
                            text: "🐧 Linux"
                            font.bold: true
                            color: ThemeColors.text
                        }

                        Label {
                            text: qsTr("• System theme integration\n• Fusion theme\n• GTK+ style support\n• Custom QSS/CSS themes\n• Desktop environment themes")
                            font.pixelSize: Theme.fontCaption
                            color: ThemeColors.textSecondary
                            Layout.fillWidth: true
                            wrapMode: Text.WordWrap
                        }
                    }
                }
            }
        }

        // Theme compatibility note
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: noteLayout.implicitHeight + Theme.spacingS * 2
            color: Qt.rgba(ThemeColors.info.r, ThemeColors.info.g, ThemeColors.info.b, 0.1)
            border.color: ThemeColors.info
            radius: Theme.radiusS

            RowLayout {
                id: noteLayout
                anchors.fill: parent
                anchors.margins: Theme.spacingS
                spacing: Theme.spacingS

                Label {
                    text: "ℹ️"
                    font.pixelSize: 16
                }

                Label {
                    text: qsTr("All built-in themes (System, Dark, Bright) work on all platforms. Platform-specific themes like Windows11 are only available on their respective platforms.")
                    font.pixelSize: Theme.fontCaption
                    color: ThemeColors.text
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }
            }
        }
    }
}
