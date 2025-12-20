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
import "../components"

WindowDialog {
    id: scrollMessageBox
    title: ""
    modal: true
    standardButtons: Dialog.Ok
    width: 500
    height: 400

    property string message: ""
    property string icon: "" // info, warning, error, question

    ColumnLayout {
        anchors.fill: parent
        spacing: Theme.spacingM

        // Icon and title row
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingM
            visible: icon.length > 0

            Rectangle {
                Layout.preferredWidth: 48
                Layout.preferredHeight: 48
                radius: 24
                color: {
                    if (icon === "info")
                        return "#3b82f6";
                    if (icon === "warning")
                        return ThemeColors.warning;
                    if (icon === "error")
                        return ThemeColors.error;
                    if (icon === "question")
                        return "#8b5cf6";
                    return ThemeColors.accent;
                }

                Label {
                    anchors.centerIn: parent
                    text: {
                        if (icon === "info")
                            return "i";
                        if (icon === "warning")
                            return "!";
                        if (icon === "error")
                            return "×";
                        if (icon === "question")
                            return "?";
                        return "";
                    }
                    color: "white"
                    font.bold: true
                    font.pointSize: 20
                }
            }

            Label {
                Layout.fillWidth: true
                text: scrollMessageBox.title
                color: ThemeColors.text
                font.bold: true
                font.pointSize: Theme.fontSizeMedium
                wrapMode: Text.WordWrap
            }
        }

        // Scrollable message content
        Frame {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ScrollView {
                anchors.fill: parent
                clip: true

                TextArea {
                    id: messageText
                    readOnly: true
                    text: message
                    wrapMode: Text.WordWrap
                    color: ThemeColors.text
                    selectByMouse: true
                    background: Rectangle {
                        color: "transparent"
                    }
                }
            }
        }

        // Copy button
        RowLayout {
            Layout.fillWidth: true

            Item {
                Layout.fillWidth: true
            }

            ThemedButton {
                text: qsTr("Copy to Clipboard")
                flatStyle: true
                size: "small"
                onClicked: {
                    if (ProjT) {
                        ProjT.copyToClipboard(message);
                    }
                }
            }
        }
    }
}
