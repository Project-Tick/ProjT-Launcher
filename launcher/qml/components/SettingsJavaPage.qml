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

ColumnLayout {
    id: javaPage
    spacing: 0

    property var vm: ProjT.launcherSettingsVM
    property var detectedJavas: []

    Connections {
        target: vm
        function onJavaAutoDetected(javaPaths) {
            detectedJavas = javaPaths;
            if (javaPaths.length > 0) {
                javaSelectionDialog.open();
            }
        }
        function onJavaTestResult(success, message) {
            javaTestResultLabel.text = message;
            javaTestResultLabel.color = success ? ThemeColors.success : ThemeColors.error;
        }
    }

    TabBar {
        id: tabBar
        Layout.fillWidth: true

        TabButton {
            text: qsTr("General")
        }
        TabButton {
            text: qsTr("Installations")
        }
    }

    StackLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true
        currentIndex: tabBar.currentIndex

        // General Tab
        ScrollView {
            clip: true

            ColumnLayout {
                width: parent.width - Theme.spacingL
                spacing: Theme.spacingM

                // Java Runtime
                GroupBox {
                    Layout.fillWidth: true
                    title: qsTr("Java Runtime")

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: Theme.spacingS

                        CheckBox {
                            id: autoDetectCheck
                            text: qsTr("Auto-detect Java")
                            checked: vm ? !vm.defaultJavaPath : true
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: Theme.spacingS
                            enabled: !autoDetectCheck.checked

                            Label {
                                text: qsTr("Java path:")
                                color: ThemeColors.text
                            }

                            TextField {
                                id: javaPathField
                                Layout.fillWidth: true
                                placeholderText: qsTr("/usr/bin/java")
                                text: vm ? vm.defaultJavaPath : ""
                                onTextChanged: if (vm && !autoDetectCheck.checked)
                                    vm.defaultJavaPath = text
                            }

                            Button {
                                text: qsTr("Browse...")
                                onClicked: browseForJava()
                            }

                            Button {
                                text: qsTr("Test")
                                onClicked: {
                                    if (vm && vm.testJavaPath) {
                                        vm.testJavaPath(javaPathField.text);
                                    }
                                }
                            }
                        }

                        Label {
                            id: javaTestResultLabel
                            text: ""
                            color: ThemeColors.textSecondary
                            font.pointSize: 9
                            wrapMode: Text.WordWrap
                            Layout.fillWidth: true
                        }

                        Button {
                            text: qsTr("Auto-detect Java installations...")
                            onClicked: {
                                if (vm && vm.autoDetectJava) {
                                    vm.autoDetectJava();
                                }
                            }
                        }
                    }
                }

                // Memory
                GroupBox {
                    Layout.fillWidth: true
                    title: qsTr("Memory")

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: Theme.spacingS

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: Theme.spacingM

                            Label {
                                text: qsTr("Minimum memory allocation:")
                                color: ThemeColors.text
                            }

                            SpinBox {
                                id: minMemSpin
                                from: 256
                                to: 65536
                                value: vm ? vm.defaultMinMemory : 512
                                stepSize: 128
                                editable: true
                                onValueModified: if (vm)
                                    vm.defaultMinMemory = value
                            }

                            Label {
                                text: qsTr("MiB")
                                color: ThemeColors.textSecondary
                            }
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: Theme.spacingM

                            Label {
                                text: qsTr("Maximum memory allocation:")
                                color: ThemeColors.text
                            }

                            SpinBox {
                                id: maxMemSpin
                                from: 256
                                to: 65536
                                value: vm ? vm.defaultMaxMemory : 4096
                                stepSize: 128
                                editable: true
                                onValueModified: if (vm)
                                    vm.defaultMaxMemory = value
                            }

                            Label {
                                text: qsTr("MiB")
                                color: ThemeColors.textSecondary
                            }
                        }

                        Label {
                            text: qsTr("Note: You generally don't need more than 4-8 GB for Minecraft")
                            color: ThemeColors.textSecondary
                            font.pointSize: 9
                            wrapMode: Text.WordWrap
                            Layout.fillWidth: true
                        }
                    }
                }

                // JVM Arguments
                GroupBox {
                    Layout.fillWidth: true
                    title: qsTr("JVM Arguments")

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: Theme.spacingS

                        TextArea {
                            id: jvmArgsArea
                            Layout.fillWidth: true
                            Layout.preferredHeight: 80
                            placeholderText: qsTr("-XX:+UseG1GC -XX:+ParallelRefProcEnabled...")
                            text: vm ? vm.defaultJvmArgs : ""
                            wrapMode: Text.Wrap
                            onTextChanged: if (vm)
                                vm.defaultJvmArgs = text
                        }

                        Label {
                            text: qsTr("Custom JVM arguments. Leave empty for defaults.")
                            color: ThemeColors.textSecondary
                            font.pointSize: 9
                        }
                    }
                }

                Item {
                    height: Theme.spacingL
                }
            }
        }

        // Installations Tab
        ColumnLayout {
            spacing: Theme.spacingS

            RowLayout {
                Layout.fillWidth: true
                Layout.margins: Theme.spacingS
                spacing: Theme.spacingS

                Button {
                    text: qsTr("Download")
                    onClicked: {
                        if (vm && vm.downloadJava) {
                            vm.downloadJava();
                        }
                    }
                }

                Button {
                    text: qsTr("Remove")
                    enabled: javaInstallationsList.currentIndex >= 0
                    onClicked: {
                        if (vm && vm.removeJavaInstallation) {
                            vm.removeJavaInstallation(javaInstallationsList.currentIndex);
                        }
                    }
                }

                Item {
                    Layout.fillWidth: true
                }

                Button {
                    text: qsTr("Refresh")
                    onClicked: {
                        if (vm && vm.refreshJavaInstallations) {
                            vm.refreshJavaInstallations();
                        }
                    }
                }
            }

            // Java installations list
            Frame {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.margins: Theme.spacingS

                ListView {
                    id: javaInstallationsList
                    anchors.fill: parent
                    clip: true
                    model: vm ? vm.javaInstallationsModel : []

                    delegate: ItemDelegate {
                        width: javaInstallationsList.width
                        height: 48
                        highlighted: ListView.isCurrentItem

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: Theme.spacingS
                            spacing: Theme.spacingS

                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 2

                                Label {
                                    text: model.name || model.version || ""
                                    color: ThemeColors.text
                                    font.bold: true
                                }

                                Label {
                                    text: model.path || ""
                                    color: ThemeColors.textSecondary
                                    font.pointSize: 9
                                    elide: Text.ElideMiddle
                                    Layout.fillWidth: true
                                }
                            }

                            Label {
                                text: model.vendor || ""
                                color: ThemeColors.textSecondary
                            }
                        }

                        onClicked: javaInstallationsList.currentIndex = index
                    }

                    ScrollBar.vertical: ScrollBar {}
                }
            }
        }
    }

    function browseForJava() {
        if (ProjT.launcherVM && ProjT.launcherVM.browseForFile) {
            var result = ProjT.launcherVM.browseForFile(qsTr("Select Java Executable"), "");
            if (result && result.length > 0) {
                javaPathField.text = result;
                if (vm)
                    vm.defaultJavaPath = result;
            }
        }
    }

    // Java selection dialog
    Dialog {
        id: javaSelectionDialog
        title: qsTr("Select Java Installation")
        standardButtons: Dialog.Ok | Dialog.Cancel
        modal: true
        anchors.centerIn: parent
        width: 500

        ColumnLayout {
            anchors.fill: parent
            spacing: Theme.spacingM

            Label {
                text: qsTr("Found %1 Java installation(s):").arg(detectedJavas.length)
                color: ThemeColors.text
            }

            ListView {
                id: detectedJavasList
                Layout.fillWidth: true
                Layout.preferredHeight: Math.min(200, detectedJavas.length * 40)
                clip: true
                model: detectedJavas

                delegate: ItemDelegate {
                    width: detectedJavasList.width
                    text: modelData
                    highlighted: ListView.isCurrentItem
                    onClicked: detectedJavasList.currentIndex = index
                }

                ScrollBar.vertical: ScrollBar {}
            }
        }

        onAccepted: {
            if (detectedJavasList.currentIndex >= 0 && detectedJavasList.currentIndex < detectedJavas.length) {
                javaPathField.text = detectedJavas[detectedJavasList.currentIndex];
                if (vm)
                    vm.defaultJavaPath = javaPathField.text;
            }
        }
    }
}
