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
    id: customCommandsWidget
    color: "transparent"

    // Properties for external binding
    property bool overrideGlobalSettings: false
    property string preLaunchCommand: ""
    property string wrapperCommand: ""
    property string postExitCommand: ""

    implicitHeight: mainLayout.implicitHeight

    ColumnLayout {
        id: mainLayout
        anchors.fill: parent
        spacing: Theme.spacingS

        // Override checkbox
        CheckBox {
            id: overrideCheckBox
            text: qsTr("Override Global Settings")
            checked: overrideGlobalSettings
            onCheckedChanged: overrideGlobalSettings = checked
        }

        // Commands container
        ColumnLayout {
            Layout.fillWidth: true
            enabled: overrideCheckBox.checked
            opacity: enabled ? 1.0 : 0.5
            spacing: Theme.spacingS

            // Pre-launch command
            Label {
                text: qsTr("Pre-launch Command")
                color: ThemeColors.text
            }

            TextField {
                id: preLaunchField
                Layout.fillWidth: true
                text: preLaunchCommand
                onTextChanged: preLaunchCommand = text
                placeholderText: qsTr("Command to run before launching")
            }

            Item {
                height: Theme.spacingXS
            }

            // Wrapper command
            Label {
                text: qsTr("Wrapper Command")
                color: ThemeColors.text
            }

            TextField {
                id: wrapperField
                Layout.fillWidth: true
                text: wrapperCommand
                onTextChanged: wrapperCommand = text
                placeholderText: qsTr("Wrapper program (e.g., optirun)")
            }

            Item {
                height: Theme.spacingXS
            }

            // Post-exit command
            Label {
                text: qsTr("Post-exit Command")
                color: ThemeColors.text
            }

            TextField {
                id: postExitField
                Layout.fillWidth: true
                text: postExitCommand
                onTextChanged: postExitCommand = text
                placeholderText: qsTr("Command to run after game exits")
            }
        }

        // Description
        Label {
            Layout.fillWidth: true
            Layout.topMargin: Theme.spacingM
            text: qsTr("<p>Pre-launch command runs before the instance launches and post-exit command runs after it exits.</p>" + "<p>Both will be run in the launcher's working folder with extra environment variables:</p>" + "<ul>" + "<li><b>$INST_NAME</b> - Name of the instance</li>" + "<li><b>$INST_ID</b> - ID of the instance (its folder name)</li>" + "<li><b>$INST_DIR</b> - absolute path of the instance</li>" + "<li><b>$INST_MC_DIR</b> - absolute path of Minecraft</li>" + "<li><b>$INST_JAVA</b> - Java binary used for launch</li>" + "<li><b>$INST_JAVA_ARGS</b> - command-line parameters used for launch</li>" + "</ul>" + "<p>Wrapper command allows launching using an extra wrapper program (like 'optirun' on Linux)</p>")
            color: ThemeColors.textSecondary
            wrapMode: Text.WordWrap
            textFormat: Text.RichText
            font.pixelSize: 11
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
