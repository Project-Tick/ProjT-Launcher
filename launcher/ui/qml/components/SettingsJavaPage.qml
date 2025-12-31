// SPDX-License-Identifier: GPL-3.0-only
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
import "."

ColumnLayout {
    id: javaPage
    spacing: ThemeColors.spacingM

    property var vm: ProjT.launcherSettingsVM
    property var detectedJavas: []

    Connections {
        target: vm
        function onJavaAutoDetected(javaPaths) {
            detectedJavas = javaPaths;
            if (javaPaths.length > 0) javaSelectionDialog.open();
        }
        function onJavaTestResult(success, message) {
            javaTestResultLabel.text = message;
            javaTestResultLabel.color = success ? ThemeColors.success : ThemeColors.error;
        }
    }

    TabBar {
        id: tabBar
        Layout.fillWidth: true
        Layout.preferredWidth: Math.min(parent.width, 400)
        Layout.alignment: Qt.AlignHCenter
        
        background: Rectangle { color: "transparent" }

        TabButton {
            text: qsTr("General")
            width: implicitWidth + 32
            
            contentItem: Text {
                text: parent.text
                font.weight: parent.checked ? Font.DemiBold : Font.Normal
                color: parent.checked ? ThemeColors.accent : ThemeColors.textSecondary
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            background: Rectangle {
                color: parent.checked ? Qt.rgba(ThemeColors.accent.r, ThemeColors.accent.g, ThemeColors.accent.b, 0.1) : "transparent"
                radius: ThemeColors.radius
            }
        }
        TabButton {
            text: qsTr("Installations")
            width: implicitWidth + 32
            
             contentItem: Text {
                text: parent.text
                font.weight: parent.checked ? Font.DemiBold : Font.Normal
                color: parent.checked ? ThemeColors.accent : ThemeColors.textSecondary
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
             background: Rectangle {
                color: parent.checked ? Qt.rgba(ThemeColors.accent.r, ThemeColors.accent.g, ThemeColors.accent.b, 0.1) : "transparent"
                radius: ThemeColors.radius
            }
        }
    }

    StackLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true
        currentIndex: tabBar.currentIndex

        // === General Tab ===
        ScrollView {
            clip: true
            contentWidth: availableWidth

            ColumnLayout {
                width: parent.width - Theme.spacingL
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: ThemeColors.spacingM

                // Java Runtime
                SettingsSection {
                    Layout.fillWidth: true
                    title: qsTr("Runtime Configuration")
                    iconSource: Theme.icon("java")

                    ColumnLayout {
                        spacing: ThemeColors.spacingS
                        Layout.fillWidth: true

                        CheckBox {
                            id: autoDetectCheck
                            text: qsTr("Auto-detect best Java version")
                            checked: vm ? !vm.defaultJavaPath : true
                        }

                        ColumnLayout {
                            visible: !autoDetectCheck.checked
                            Layout.fillWidth: true
                            spacing: ThemeColors.spacingXS

                            Label {
                                text: qsTr("Java executable path")
                                color: ThemeColors.textSecondary
                                font.pixelSize: 12
                            }
                            
                            RowLayout {
                                Layout.fillWidth: true
                                spacing: ThemeColors.spacingS

                                TextField {
                                    id: javaPathField
                                    Layout.fillWidth: true
                                    placeholderText: qsTr("/usr/bin/java")
                                    text: vm ? vm.defaultJavaPath : ""
                                    onTextChanged: if(vm && !autoDetectCheck.checked) vm.defaultJavaPath = text
                                }
                                ThemedButton {
                                    text: "📂"
                                    onClicked: browseForJava()
                                    ToolTip.visible: hovered
                                    ToolTip.text: qsTr("Browse")
                                }
                                ThemedButton {
                                    text: qsTr("Test")
                                    outline: true
                                    onClicked: if(vm && vm.testJavaPath) vm.testJavaPath(javaPathField.text)
                                }
                            }
                        }

                        Label {
                            id: javaTestResultLabel
                            visible: text !== ""
                            wrapMode: Text.WordWrap
                            Layout.fillWidth: true
                        }

                        ThemedButton {
                            text: qsTr("Scan for Java installations...")
                            outline: true
                            onClicked: if(vm && vm.autoDetectJava) vm.autoDetectJava()
                        }
                    }
                }

                // Memory
                SettingsSection {
                    Layout.fillWidth: true
                    title: qsTr("Memory Allocation")
                    iconSource: Theme.icon("settings")

                    GridLayout {
                        columns: 2
                        columnSpacing: ThemeColors.spacingXL
                        rowSpacing: ThemeColors.spacingM
                        Layout.fillWidth: true

                        Label {
                            text: qsTr("Minimum (MiB):")
                            color: ThemeColors.textSecondary
                        }
                        SpinBox {
                            id: minMemSpin
                            from: 256; to: 65536
                            value: vm ? vm.defaultMinMemory : 512
                            stepSize: 128
                            editable: true
                            onValueModified: if(vm) vm.defaultMinMemory = value
                        }

                        Label {
                            text: qsTr("Maximum (MiB):")
                            color: ThemeColors.textSecondary
                        }
                        SpinBox {
                            id: maxMemSpin
                            from: 256; to: 65536
                            value: vm ? vm.defaultMaxMemory : 4096
                            stepSize: 128
                            editable: true
                            onValueModified: if(vm) vm.defaultMaxMemory = value
                            
                            // Add a validator or warning visual if needed
                        }
                    }
                    
                    Label {
                        text: qsTr("Tip: 4096 MiB (4GB) is recommended for most modpacks.")
                        color: ThemeColors.textSecondary
                        font.italic: true
                        font.pixelSize: 12
                    }
                }

                // JVM Arguments
                SettingsSection {
                    Layout.fillWidth: true
                    title: qsTr("JVM Arguments")
                    iconSource: Theme.icon("custom-commands")

                    ColumnLayout {
                        spacing: ThemeColors.spacingS
                        Layout.fillWidth: true

                        TextArea {
                            id: jvmArgsArea
                            Layout.fillWidth: true
                            Layout.preferredHeight: 80
                            placeholderText: "-XX:+UseG1GC..."
                            text: vm ? vm.defaultJvmArgs : ""
                            wrapMode: Text.Wrap
                            background: Rectangle {
                                color: ThemeColors.bg1
                                radius: ThemeColors.radiusS
                                border.color: parent.activeFocus ? ThemeColors.accent : ThemeColors.border
                            }
                            onTextChanged: if(vm) vm.defaultJvmArgs = text
                        }
                         Label {
                            text: qsTr("Leave empty to use recommended defaults.")
                            color: ThemeColors.textSecondary
                            font.pixelSize: 11
                        }
                    }
                }
                
                Item { height: ThemeColors.spacingL }
            }
        }

        // === Installations Tab ===
        ColumnLayout {
            spacing: ThemeColors.spacingM

            RowLayout {
                Layout.fillWidth: true
                Layout.margins: ThemeColors.spacingM
                spacing: ThemeColors.spacingS

                ThemedButton {
                    text: qsTr("Download Java")
                    onClicked: if(vm && vm.downloadJava) vm.downloadJava()
                }

                ThemedButton {
                    text: qsTr("Remove")
                    outline: true
                    enabled: javaInstallationsList.currentIndex >= 0
                    onClicked: if(vm && vm.removeJavaInstallation) vm.removeJavaInstallation(javaInstallationsList.currentIndex)
                }

                Item { Layout.fillWidth: true }

                ThemedButton {
                    text: qsTr("Refresh")
                    flatStyle: true
                    onClicked: if(vm && vm.refreshJavaInstallations) vm.refreshJavaInstallations()
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.margins: ThemeColors.spacingM
                color: ThemeColors.bg1
                radius: ThemeColors.radius
                border.color: ThemeColors.border

                ListView {
                    id: javaInstallationsList
                    anchors.fill: parent
                    anchors.margins: 4
                    clip: true
                    model: vm ? vm.javaInstallationsModel : []

                    delegate: ItemDelegate {
                        width: javaInstallationsList.width
                        height: 50
                        highlighted: ListView.isCurrentItem
                        
                        background: Rectangle {
                            color: parent.highlighted ? ThemeColors.surfaceHighlight : "transparent"
                            radius: ThemeColors.radiusS
                        }

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: ThemeColors.spacingS
                            spacing: ThemeColors.spacingS

                            Text {
                                text: "☕"
                                font.pixelSize: 16
                            }

                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 0
                                Label {
                                    text: model.name || model.version || "Unknown Version"
                                    color: ThemeColors.textTitle
                                    font.bold: true
                                }
                                Label {
                                    text: model.path || ""
                                    color: ThemeColors.textSecondary
                                    font.pixelSize: 11
                                    elide: Text.ElideMiddle
                                    Layout.fillWidth: true
                                }
                            }
                            
                            Label {
                                text: model.vendor || ""
                                color: ThemeColors.textDisabled
                                font.pixelSize: 11
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
                if (vm) vm.defaultJavaPath = result;
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
        parent: Overlay.overlay

        background: Rectangle {
            color: ThemeColors.surface
            border.color: ThemeColors.border
            radius: ThemeColors.radius
        }

        ColumnLayout {
            anchors.fill: parent
            spacing: ThemeColors.spacingM

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
                if (vm) vm.defaultJavaPath = javaPathField.text;
            }
        }
    }
}

