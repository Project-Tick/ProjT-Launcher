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
    id: customMessageBox
    modal: true
    standardButtons: Dialog.NoButton
    width: 400
    height: contentHeight + 120

    property string message: ""
    property string icon: "" // info, warning, error, question, success
    property var buttons: [] // Array of { text: "Button", role: "accept/reject/custom", highlighted: bool }
    property int contentHeight: Math.max(messageLabel.implicitHeight + iconRect.height, 100)

    signal buttonClicked(string role)

    ColumnLayout {
        anchors.fill: parent
        spacing: Theme.spacingM

        // Icon
        Rectangle {
            id: iconRect
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: 64
            Layout.preferredHeight: 64
            radius: 32
            visible: icon.length > 0
            color: {
                if (icon === "info")
                    return "#3b82f6";
                if (icon === "warning")
                    return ThemeColors.warning;
                if (icon === "error")
                    return ThemeColors.error;
                if (icon === "question")
                    return "#8b5cf6";
                if (icon === "success")
                    return ThemeColors.success;
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
                    if (icon === "success")
                        return "✓";
                    return "";
                }
                color: "white"
                font.bold: true
                font.pointSize: 28
            }
        }

        // Title
        Label {
            Layout.fillWidth: true
            text: customMessageBox.title
            color: ThemeColors.text
            font.bold: true
            font.pointSize: Theme.fontSizeMedium
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            visible: customMessageBox.title.length > 0
        }

        // Message
        Label {
            id: messageLabel
            Layout.fillWidth: true
            Layout.fillHeight: true
            text: message
            color: ThemeColors.textSecondary
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
        }

        // Buttons
        RowLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
            spacing: Theme.spacingS

            Repeater {
                model: buttons

                delegate: Button {
                    text: modelData.text
                    highlighted: modelData.highlighted === true

                    onClicked: {
                        buttonClicked(modelData.role || "custom");
                        if (modelData.role === "accept") {
                            customMessageBox.accept();
                        } else if (modelData.role === "reject") {
                            customMessageBox.reject();
                        } else {
                            customMessageBox.close();
                        }
                    }
                }
            }
        }
    }

    // Convenience factory functions
    function showInfo(title, message) {
        customMessageBox.title = title;
        customMessageBox.message = message;
        customMessageBox.icon = "info";
        customMessageBox.buttons = [
            {
                text: qsTr("OK"),
                role: "accept",
                highlighted: true
            }
        ];
        customMessageBox.open();
    }

    function showWarning(title, message) {
        customMessageBox.title = title;
        customMessageBox.message = message;
        customMessageBox.icon = "warning";
        customMessageBox.buttons = [
            {
                text: qsTr("OK"),
                role: "accept",
                highlighted: true
            }
        ];
        customMessageBox.open();
    }

    function showError(title, message) {
        customMessageBox.title = title;
        customMessageBox.message = message;
        customMessageBox.icon = "error";
        customMessageBox.buttons = [
            {
                text: qsTr("OK"),
                role: "accept",
                highlighted: true
            }
        ];
        customMessageBox.open();
    }

    function showQuestion(title, message, yesText, noText) {
        customMessageBox.title = title;
        customMessageBox.message = message;
        customMessageBox.icon = "question";
        customMessageBox.buttons = [
            {
                text: noText || qsTr("No"),
                role: "reject"
            },
            {
                text: yesText || qsTr("Yes"),
                role: "accept",
                highlighted: true
            }
        ];
        customMessageBox.open();
    }

    function showSuccess(title, message) {
        customMessageBox.title = title;
        customMessageBox.message = message;
        customMessageBox.icon = "success";
        customMessageBox.buttons = [
            {
                text: qsTr("OK"),
                role: "accept",
                highlighted: true
            }
        ];
        customMessageBox.open();
    }
}
