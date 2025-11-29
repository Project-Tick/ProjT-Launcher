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

Rectangle {
    color: "#1b1b1b"
    width: parent ? parent.width : 640
    height: parent ? parent.height : 480

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        Label {
            text: qsTr("Java Settings")
            color: "#eceff1"
            font.pixelSize: 16
            font.bold: true
        }

        CheckBox {
            text: qsTr("Override Java location")
            checked: ProjT.settingsVM ? ProjT.settingsVM.overrideJavaLocation : false
            onToggled: {
                if (ProjT.settingsVM) {
                    ProjT.settingsVM.overrideJavaLocation = checked
                }
            }
            enabled: ProjT.settingsVM && !ProjT.settingsVM.saveBusy
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 8
            Label {
                text: qsTr("Java path")
                color: "#e0e0e0"
                Layout.alignment: Qt.AlignVCenter
            }
            TextField {
                Layout.fillWidth: true
                text: ProjT.settingsVM ? ProjT.settingsVM.javaPath : ""
                onTextChanged: {
                    if (ProjT.settingsVM) {
                        ProjT.settingsVM.javaPath = text
                    }
                }
                enabled: ProjT.settingsVM && !ProjT.settingsVM.saveBusy
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 8
            Button {
                text: qsTr("Save")
                implicitHeight: 34
                implicitWidth: 90
                Layout.alignment: Qt.AlignVCenter
                enabled: ProjT.settingsVM && !ProjT.settingsVM.saveBusy
                onClicked: ProjT.settingsVM ? ProjT.settingsVM.saveAll() : undefined
            }
            Button {
                text: qsTr("Reset")
                implicitHeight: 34
                implicitWidth: 90
                Layout.alignment: Qt.AlignVCenter
                enabled: ProjT.settingsVM && !ProjT.settingsVM.saveBusy
                onClicked: ProjT.settingsVM ? ProjT.settingsVM.resetToDefaultsForCurrentCategory() : undefined
            }
            Rectangle { Layout.fillWidth: true; color: "transparent" }
            BusyIndicator {
                running: ProjT.settingsVM ? ProjT.settingsVM.saveBusy : false
                visible: running
            }
        }

        Label {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            color: "#ef5350"
            text: ProjT.settingsVM ? ProjT.settingsVM.lastErrorMessage : ""
            visible: text.length > 0
        }
    }
}
