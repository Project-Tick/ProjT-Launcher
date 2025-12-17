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
    id: externalToolsPage
    clip: true

    property var vm: ProjT.launcherSettingsVM

    ColumnLayout {
        width: externalToolsPage.width - Theme.spacingL
        spacing: Theme.spacingM

        // Editors
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("&Editors")

            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS

                Label {
                    text: qsTr("&Text Editor")
                    color: ThemeColors.text
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingS

                    TextField {
                        id: jsonEditorTextBox
                        Layout.fillWidth: true
                        text: vm ? vm.jsonEditorPath : ""
                        onTextChanged: if (vm)
                            vm.jsonEditorPath = text
                    }

                    Button {
                        text: qsTr("Browse")
                        onClicked: browseForJsonEditor()
                    }
                }

                Label {
                    text: qsTr("Used to edit component JSON files.")
                    color: ThemeColors.textSecondary
                }

                Item {
                    height: 6
                }

                Label {
                    text: qsTr("&MCEdit")
                    color: ThemeColors.text
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingS

                    TextField {
                        id: mceditPathEdit
                        Layout.fillWidth: true
                        text: vm ? vm.mceditPath : ""
                        onTextChanged: if (vm)
                            vm.mceditPath = text
                    }

                    Button {
                        text: qsTr("Browse")
                        onClicked: browseForMCEdit()
                    }
                }

                Button {
                    text: qsTr("Check")
                    onClicked: checkMCEdit()
                }

                Label {
                    text: "<a href='https://www.mcedit.net/'>MCEdit Website</a> - Used as world editor in the instance Worlds menu."
                    textFormat: Text.RichText
                    color: ThemeColors.text
                    onLinkActivated: Qt.openUrlExternally(link)

                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.NoButton
                        cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                    }
                }
            }
        }

        // Profilers
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("&Profilers")

            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS

                Label {
                    text: qsTr("Profilers are accessible through the Launch dropdown menu.")
                    color: ThemeColors.text
                }

                Item {
                    height: 6
                }

                Label {
                    text: qsTr("J&Profiler")
                    color: ThemeColors.text
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingS

                    TextField {
                        id: jprofilerPathEdit
                        Layout.fillWidth: true
                        text: vm ? vm.jprofilerPath : ""
                        onTextChanged: if (vm)
                            vm.jprofilerPath = text
                    }

                    Button {
                        text: qsTr("Browse")
                        onClicked: browseForJProfiler()
                    }
                }

                Button {
                    text: qsTr("Check")
                    onClicked: checkJProfiler()
                }

                Label {
                    text: "<a href='https://www.ej-technologies.com/products/jprofiler/overview.html'>JProfiler Website</a>"
                    textFormat: Text.RichText
                    color: ThemeColors.text
                    onLinkActivated: Qt.openUrlExternally(link)

                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.NoButton
                        cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                    }
                }

                Item {
                    height: 6
                }

                Label {
                    text: qsTr("&VisualVM")
                    color: ThemeColors.text
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingS

                    TextField {
                        id: jvisualvmPathEdit
                        Layout.fillWidth: true
                        text: vm ? vm.jvisualvmPath : ""
                        onTextChanged: if (vm)
                            vm.jvisualvmPath = text
                    }

                    Button {
                        text: qsTr("Browse")
                        onClicked: browseForJVisualVM()
                    }
                }

                Button {
                    text: qsTr("Check")
                    onClicked: checkJVisualVM()
                }

                Label {
                    text: "<a href='https://visualvm.github.io/'>VisualVM Website</a>"
                    textFormat: Text.RichText
                    color: ThemeColors.text
                    onLinkActivated: Qt.openUrlExternally(link)

                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.NoButton
                        cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                    }
                }
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }

    // Browse functions
    function browseForJsonEditor() {
        if (ProjT.launcherVM && ProjT.launcherVM.browseForFile) {
            var result = ProjT.launcherVM.browseForFile(qsTr("Select Text Editor"), "");
            if (result && result.length > 0) {
                jsonEditorTextBox.text = result;
                if (vm)
                    vm.jsonEditorPath = result;
            }
        }
    }

    function browseForMCEdit() {
        if (ProjT.launcherVM && ProjT.launcherVM.browseForFile) {
            var result = ProjT.launcherVM.browseForFile(qsTr("Select MCEdit Executable"), "");
            if (result && result.length > 0) {
                mceditPathEdit.text = result;
                if (vm)
                    vm.mceditPath = result;
            }
        }
    }

    function browseForJProfiler() {
        if (ProjT.launcherVM && ProjT.launcherVM.browseForFile) {
            var result = ProjT.launcherVM.browseForFile(qsTr("Select JProfiler Executable"), "");
            if (result && result.length > 0) {
                jprofilerPathEdit.text = result;
                if (vm)
                    vm.jprofilerPath = result;
            }
        }
    }

    function browseForJVisualVM() {
        if (ProjT.launcherVM && ProjT.launcherVM.browseForFile) {
            var result = ProjT.launcherVM.browseForFile(qsTr("Select VisualVM Executable"), "");
            if (result && result.length > 0) {
                jvisualvmPathEdit.text = result;
                if (vm)
                    vm.jvisualvmPath = result;
            }
        }
    }

    // Check functions
    function checkMCEdit() {
        if (vm && vm.checkTool) {
            vm.checkTool("mcedit", mceditPathEdit.text);
        }
    }

    function checkJProfiler() {
        if (vm && vm.checkTool) {
            vm.checkTool("jprofiler", jprofilerPathEdit.text);
        }
    }

    function checkJVisualVM() {
        if (vm && vm.checkTool) {
            vm.checkTool("jvisualvm", jvisualvmPathEdit.text);
        }
    }
}
