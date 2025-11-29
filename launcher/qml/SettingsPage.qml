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
import "components"

Rectangle {
    objectName: "settings"
    color: "#1b1b1b"
    width: parent ? parent.width : 640
    height: parent ? parent.height : 480

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        PageHeader {
            Layout.fillWidth: true
            title: qsTr("Instance Settings")
            subtitle: qsTr("Manage configuration for the selected instance")
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 12

            ListView {
                id: categoryList
                Layout.preferredWidth: 180
                Layout.fillHeight: true
                model: ProjT.settingsVM ? ProjT.settingsVM.categoryList : ["Java"]
                delegate: Rectangle {
                    width: categoryList.width
                    height: 36
                    color: (ProjT.settingsVM && ProjT.settingsVM.currentCategory === modelData.toLowerCase()) ? "#2c3440" : "#23262b"
                    border.color: "#323742"
                    radius: 4
                    Text {
                        anchors.centerIn: parent
                        text: modelData
                        color: "#e0e0e0"
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if (ProjT.settingsVM) {
                                ProjT.settingsVM.loadCategory(modelData.toLowerCase())
                            }
                        }
                    }
                }
            }

            Loader {
                id: categoryLoader
                Layout.fillWidth: true
                Layout.fillHeight: true
                source: ProjT.settingsVM && ProjT.settingsVM.currentCategory === "java"
                        ? Qt.resolvedUrl("settings/JavaSettingsPage.qml")
                        : ""
            }
        }
        RowLayout {
            Layout.fillWidth: true
            spacing: 8
            Button {
                text: qsTr("Apply")
                enabled: ProjT.settingsVM && !ProjT.settingsVM.busy
                onClicked: { if (ProjT.settingsVM) ProjT.settingsVM.applyChanges() }
            }
            Button {
                text: qsTr("Reset")
                enabled: ProjT.settingsVM && !ProjT.settingsVM.busy
                onClicked: { if (ProjT.settingsVM) ProjT.settingsVM.resetChanges() }
            }
            Rectangle { Layout.fillWidth: true; color: "transparent" }
        }
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#000000"
            opacity: ProjT.settingsVM && ProjT.settingsVM.busy ? 0.25 : 0
            visible: opacity > 0
            Behavior on opacity { NumberAnimation { duration: 150 } }
            BusyIndicator {
                anchors.centerIn: parent
                running: ProjT.settingsVM ? ProjT.settingsVM.busy : false
                visible: running
            }
        }
    }

    Component.onCompleted: {
        if (ProjT.settingsVM) {
            if (!ProjT.settingsVM.currentCategory || ProjT.settingsVM.currentCategory.length === 0) {
                ProjT.settingsVM.currentCategory = "java"
            }
            ProjT.settingsVM.refresh()
        }
    }
}
