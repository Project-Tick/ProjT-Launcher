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
import "../components"

Rectangle {
    id: loginPage
    color: ThemeColors.background

    property var vm: ProjT.accountsVM
    property bool accountAdded: false

    signal loginRequested
    signal accountAddedChanged

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingL
        spacing: Theme.spacingM

        // Title
        Label {
            text: qsTr("Add Microsoft account")
            font.pixelSize: 18
            font.bold: true
            color: ThemeColors.text
        }

        // Description
        Label {
            Layout.fillWidth: true
            text: qsTr("In order to play Minecraft, you must have at least one Microsoft account logged in. Do you want to log in now?")
            color: ThemeColors.text
            wrapMode: Text.WordWrap
        }

        // Separator
        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: ThemeColors.border
        }

        // Login button
        ThemedButton {
            Layout.fillWidth: true
            text: qsTr("Add Microsoft account")
            primary: true
            size: "large"
            onClicked: {
                loginRequested();
                if (vm)
                    vm.addMicrosoftAccount();
            }
        }

        // Status
        RowLayout {
            Layout.fillWidth: true
            visible: accountAdded
            spacing: Theme.spacingS

            Label {
                text: "✓"
                color: ThemeColors.success
                font.bold: true
                font.pixelSize: 16
            }

            Label {
                text: qsTr("Account added successfully!")
                color: ThemeColors.success
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }

    Connections {
        target: vm
        ignoreUnknownSignals: true
        function onAccountAdded() {
            accountAdded = true;
            accountAddedChanged();
        }
    }
}
