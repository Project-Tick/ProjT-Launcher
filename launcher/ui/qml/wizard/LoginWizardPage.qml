// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import ProjTLauncher 1.0
import "../Theme.js" as Theme
import "../components"

Rectangle {
    id: loginPage
    color: ThemeColors.bg

    property var vm: ProjT.accountsVM
    property bool accountAdded: false

    signal loginRequested
    signal accountAddedSignal

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: ThemeColors.spacingM
        spacing: ThemeColors.spacingS

        // Title
        Label {
            text: qsTr("Add Microsoft account")
            font: ThemeColors.fontTitle
            color: ThemeColors.text
        }

        // Description
        Label {
            Layout.fillWidth: true
            text: qsTr("In order to play Minecraft, you must have at least one Microsoft account logged in. Do you want to log in now?")
            color: ThemeColors.textSecondary
            wrapMode: Text.WordWrap
            font: ThemeColors.fontBody
        }

        // Separator
        Rectangle {
            Layout.fillWidth: true
            Layout.topMargin: 10
            Layout.bottomMargin: 10
            height: 1
            color: ThemeColors.border
        }

        // Login button
        AppButton {
            Layout.alignment: Qt.AlignLeft
            text: qsTr("Add Microsoft account")
            variant: "primary"
            onClicked: {
                loginRequested();
                if (vm)
                    vm.addMicrosoftAccount();
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
            accountAddedSignal();
        }
    }
}
