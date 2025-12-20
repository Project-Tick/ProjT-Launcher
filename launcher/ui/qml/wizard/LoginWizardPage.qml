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
