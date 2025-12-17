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

ScrollView {
    id: proxyPage
    clip: true

    property var vm: ProjT.launcherSettingsVM

    ColumnLayout {
        width: proxyPage.width - Theme.spacingL
        spacing: Theme.spacingM

        // Warning Label
        Label {
            text: qsTr("This only applies to the launcher. Minecraft does not accept proxy settings.")
            color: ThemeColors.text
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }

        // Proxy Type
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Type")

            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS

                RadioButton {
                    id: proxyDefaultBtn
                    text: qsTr("Use s&ystem settings")
                    ToolTip.text: qsTr("Uses your system's default proxy settings.")
                    ToolTip.visible: hovered
                    checked: vm ? vm.proxyType === "default" : true
                    onCheckedChanged: if (vm && checked)
                        vm.proxyType = "default"
                }

                RadioButton {
                    id: proxyNoneBtn
                    text: qsTr("&None")
                    checked: vm ? vm.proxyType === "none" : false
                    onCheckedChanged: if (vm && checked)
                        vm.proxyType = "none"
                }

                RadioButton {
                    id: proxySOCKS5Btn
                    text: qsTr("&SOCKS5")
                    checked: vm ? vm.proxyType === "socks5" : false
                    onCheckedChanged: if (vm && checked)
                        vm.proxyType = "socks5"
                }

                RadioButton {
                    id: proxyHTTPBtn
                    text: qsTr("&HTTP")
                    checked: vm ? vm.proxyType === "http" : false
                    onCheckedChanged: if (vm && checked)
                        vm.proxyType = "http"
                }
            }
        }

        // Address and Port
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("&Address and Port")
            enabled: proxySOCKS5Btn.checked || proxyHTTPBtn.checked

            RowLayout {
                anchors.fill: parent
                spacing: Theme.spacingS

                TextField {
                    id: proxyAddrEdit
                    Layout.minimumWidth: 300
                    Layout.preferredWidth: 300
                    placeholderText: "127.0.0.1"
                    text: vm ? vm.proxyHost : ""
                    onTextChanged: if (vm)
                        vm.proxyHost = text
                }

                SpinBox {
                    id: proxyPortEdit
                    from: 1
                    to: 65535
                    value: vm ? vm.proxyPort : 8080
                    editable: true
                    onValueModified: if (vm)
                        vm.proxyPort = value
                }

                Item {
                    Layout.fillWidth: true
                }
            }
        }

        // Authentication
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Authentication")
            enabled: proxySOCKS5Btn.checked || proxyHTTPBtn.checked

            GridLayout {
                anchors.fill: parent
                columns: 2
                rowSpacing: Theme.spacingS
                columnSpacing: Theme.spacingM

                Label {
                    text: qsTr("&Username:")
                    color: ThemeColors.text
                }

                TextField {
                    id: proxyUserEdit
                    Layout.fillWidth: true
                    text: vm ? vm.proxyUsername : ""
                    onTextChanged: if (vm)
                        vm.proxyUsername = text
                }

                Label {
                    text: qsTr("&Password:")
                    color: ThemeColors.text
                }

                TextField {
                    id: proxyPassEdit
                    Layout.fillWidth: true
                    echoMode: TextInput.Password
                    text: vm ? vm.proxyPassword : ""
                    onTextChanged: if (vm)
                        vm.proxyPassword = text
                }

                Label {
                    text: qsTr("Note: Proxy username and password are stored in plain text inside the launcher's configuration file!")
                    color: ThemeColors.text
                    wrapMode: Text.WordWrap
                    Layout.columnSpan: 2
                    Layout.fillWidth: true
                }
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
