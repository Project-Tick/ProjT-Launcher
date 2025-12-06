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

Dialog {
    id: reviewMessageBox
    title: qsTr("Review Changes")
    modal: true
    standardButtons: Dialog.Yes | Dialog.No
    width: 500
    height: 400

    property string message: ""
    property var changes: []
    property string acceptText: qsTr("Proceed")
    property string rejectText: qsTr("Cancel")

    ColumnLayout {
        anchors.fill: parent
        spacing: Theme.spacingM

        // Message
        Label {
            Layout.fillWidth: true
            text: message
            color: ThemeColors.text
            wrapMode: Text.WordWrap
        }

        // Changes list
        GroupBox {
            Layout.fillWidth: true
            Layout.fillHeight: true
            title: qsTr("Changes")

            Frame {
                anchors.fill: parent

                ListView {
                    id: changesList
                    anchors.fill: parent
                    clip: true
                    model: changes
                    spacing: 2

                    delegate: Rectangle {
                        width: changesList.width
                        height: changeRow.implicitHeight + Theme.spacingS * 2
                        radius: 4
                        color: {
                            if (modelData.type === "add")
                                return "#22c55e15";
                            if (modelData.type === "remove")
                                return "#ef444415";
                            if (modelData.type === "update")
                                return Qt.rgba(ThemeColors.warning.r, ThemeColors.warning.g, ThemeColors.warning.b, 0.08);
                            return "transparent";
                        }

                        RowLayout {
                            id: changeRow
                            anchors.fill: parent
                            anchors.margins: Theme.spacingS
                            spacing: Theme.spacingS

                            // Type indicator
                            Rectangle {
                                Layout.preferredWidth: 24
                                Layout.preferredHeight: 24
                                radius: 12
                                color: {
                                    if (modelData.type === "add")
                                        return ThemeColors.success;
                                    if (modelData.type === "remove")
                                        return ThemeColors.error;
                                    if (modelData.type === "update")
                                        return ThemeColors.warning;
                                    return ThemeColors.textSecondary;
                                }

                                Label {
                                    anchors.centerIn: parent
                                    text: {
                                        if (modelData.type === "add")
                                            return "+";
                                        if (modelData.type === "remove")
                                            return "-";
                                        if (modelData.type === "update")
                                            return "↑";
                                        return "?";
                                    }
                                    color: "white"
                                    font.bold: true
                                }
                            }

                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 2

                                Label {
                                    text: modelData.name || qsTr("Unknown")
                                    color: ThemeColors.text
                                    font.bold: true
                                }

                                Label {
                                    text: modelData.description || ""
                                    color: ThemeColors.textSecondary
                                    font.pointSize: Theme.fontSizeSmall
                                    visible: text.length > 0
                                    wrapMode: Text.WordWrap
                                    Layout.fillWidth: true
                                }
                            }
                        }
                    }

                    ScrollBar.vertical: ScrollBar {}
                }

                Label {
                    anchors.centerIn: parent
                    text: qsTr("No changes to review")
                    color: ThemeColors.textSecondary
                    visible: changes.length === 0
                }
            }
        }

        // Summary
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingM

            Label {
                text: qsTr("%1 change(s)").arg(changes.length)
                color: ThemeColors.textSecondary
            }

            Item {
                Layout.fillWidth: true
            }

            // Count by type
            RowLayout {
                spacing: Theme.spacingS
                visible: changes.length > 0

                Label {
                    text: "+" + changes.filter(function (c) {
                        return c.type === "add";
                    }).length
                    color: ThemeColors.success
                    font.bold: true
                }

                Label {
                    text: "-" + changes.filter(function (c) {
                        return c.type === "remove";
                    }).length
                    color: ThemeColors.error
                    font.bold: true
                }

                Label {
                    text: "↑" + changes.filter(function (c) {
                        return c.type === "update";
                    }).length
                    color: ThemeColors.warning
                    font.bold: true
                }
            }
        }
    }
}
