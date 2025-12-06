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
    id: profileSelectDialog
    title: qsTr("Select Profile")
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel
    width: 400
    height: 400

    property string selectedProfile: ""
    property var vm: ProjT ? ProjT.accountsVM : null

    ColumnLayout {
        anchors.fill: parent
        spacing: Theme.spacingS

        Label {
            text: qsTr("Select an account to use:")
            color: ThemeColors.textSecondary
        }

        Frame {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ListView {
                id: profileList
                anchors.fill: parent
                clip: true
                model: vm ? vm.accounts : []

                delegate: ItemDelegate {
                    width: profileList.width
                    height: 56
                    highlighted: modelData.id === selectedProfile

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: Theme.spacingS
                        spacing: Theme.spacingM

                        // Avatar
                        Rectangle {
                            Layout.preferredWidth: 40
                            Layout.preferredHeight: 40
                            radius: 4
                            color: ThemeColors.backgroundAlt

                            Image {
                                anchors.fill: parent
                                anchors.margins: 2
                                source: modelData.avatarUrl || ""
                                fillMode: Image.PreserveAspectFit
                                visible: status === Image.Ready
                            }

                            Label {
                                anchors.centerIn: parent
                                text: modelData.username ? modelData.username.charAt(0).toUpperCase() : "?"
                                font.pointSize: 16
                                color: ThemeColors.textSecondary
                                visible: parent.children[0].status !== Image.Ready
                            }
                        }

                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2

                            Label {
                                text: modelData.username || qsTr("Unknown")
                                color: ThemeColors.text
                                font.bold: modelData.isActive
                            }

                            Label {
                                text: {
                                    if (modelData.type === "msa")
                                        return "Microsoft Account";
                                    if (modelData.type === "offline")
                                        return qsTr("Offline");
                                    return modelData.type || "";
                                }
                                color: ThemeColors.textSecondary
                                font.pointSize: Theme.fontSizeSmall
                            }
                        }

                        // Active indicator
                        Rectangle {
                            Layout.preferredWidth: 8
                            Layout.preferredHeight: 8
                            radius: 4
                            color: ThemeColors.accent
                            visible: modelData.isActive
                        }
                    }

                    onClicked: {
                        selectedProfile = modelData.id;
                    }

                    onDoubleClicked: {
                        selectedProfile = modelData.id;
                        profileSelectDialog.accept();
                    }
                }

                ScrollBar.vertical: ScrollBar {}
            }

            Label {
                anchors.centerIn: parent
                text: qsTr("No accounts added")
                color: ThemeColors.textSecondary
                visible: !vm || vm.accounts.length === 0
            }
        }

        // Add account button
        Button {
            Layout.alignment: Qt.AlignRight
            text: qsTr("Add Account")
            icon.name: "list-add"
            onClicked: {
                if (vm)
                    vm.addAccount();
            }
        }
    }
}
