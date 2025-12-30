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
 *
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import ProjTLauncher 1.0
import "components"
import "Theme.js" as Theme

Rectangle {
    objectName: "settings"
    color: ThemeColors.background
    width: parent ? parent.width : 640
    height: parent ? parent.height : 480

    property var vm: ProjT.settingsVM
    property var categoriesModel: vm && vm.categoryList && vm.categoryList.length ? vm.categoryList : ["java", "memory", "args", "commands", "env", "loader", "game", "notes", "icon", "overrides"]

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
                spacing: 6
                model: categoriesModel
                delegate: Rectangle {
                    width: categoryList.width
                    height: 36
                    color: (vm && vm.currentCategory === modelData) ? ThemeColors.highlight : ThemeColors.surface
                    border.color: ThemeColors.border
                    radius: 6
                    Text {
                        anchors.centerIn: parent
                        text: modelData.charAt(0).toUpperCase() + modelData.slice(1)
                        color: ThemeColors.text
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if (vm) {
                                vm.loadCategory(modelData);
                            }
                        }
                    }
                }
            }

            Frame {
                Layout.fillWidth: true
                Layout.fillHeight: true
                padding: 10

                StackLayout {
                    id: stack
                    anchors.fill: parent
                    currentIndex: {
                        if (!vm)
                            return 0;
                        const idx = categoriesModel.indexOf(vm.currentCategory);
                        return idx >= 0 ? idx : 0;
                    }

                    Repeater {
                        model: categoriesModel.length
                        delegate: Loader {
                            property string categoryName: categoriesModel[index]
                            sourceComponent: categoryComponent(categoryName)
                        }
                    }
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 8
            Button {
                text: qsTr("Apply")
                enabled: vm && !vm.busy
                onClicked: {
                    if (vm)
                        vm.applyChanges();
                }
            }
            Button {
                text: qsTr("Reset")
                enabled: vm && !vm.busy
                onClicked: {
                    if (vm)
                        vm.resetChanges();
                }
            }
            Rectangle {
                Layout.fillWidth: true
                color: "transparent"
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        color: ThemeColors.background
        opacity: vm && vm.busy ? 0.25 : 0
        visible: opacity > 0
        Behavior on opacity {
            NumberAnimation {
                duration: 150
            }
        }
        Column {
            anchors.centerIn: parent
            spacing: 6
            BusyIndicator {
                running: vm ? vm.busy : false
                visible: running
            }
            Label {
                text: vm && vm.busyReason ? vm.busyReason : ""
                color: ThemeColors.text
                visible: text.length > 0
            }
        }
    }

    Component.onCompleted: {
        if (vm) {
            if (!vm.currentCategory || vm.currentCategory.length === 0) {
                vm.currentCategory = "java";
            }
            vm.refresh();
        }
    }

    function categoryComponent(name) {
        switch (name) {
        case "java":
            return javaCategory;
        case "memory":
            return memoryCategory;
        case "args":
            return argsCategory;
        case "commands":
            return commandsCategory;
        case "env":
            return envCategory;
        case "loader":
            return loaderCategory;
        case "game":
            return gameCategory;
        case "notes":
            return notesCategory;
        case "icon":
            return iconCategory;
        case "overrides":
            return overridesCategory;
        default:
            return placeholderCategory;
        }
    }

    Component {
        id: placeholderCategory
        Flickable {
            contentWidth: width
            contentHeight: 200
            clip: true
            ColumnLayout {
                anchors.fill: parent
                spacing: 8
                Label {
                    text: qsTr("This category is not yet ported to QML.")
                    color: ThemeColors.text
                }
            }
        }
    }

    Component {
        id: javaCategory
        Flickable {
            contentWidth: width
            contentHeight: 360
            clip: true
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 4
                spacing: 10
                Label {
                    text: qsTr("Java Runtime")
                    color: ThemeColors.text
                    font.bold: true
                }
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 8
                    enabled: vm ? vm.overrideJavaLocation : false
                    TextField {
                        id: javaPathField
                        Layout.fillWidth: true
                        placeholderText: qsTr("Java path")
                        text: vm ? vm.javaPath : ""
                        onEditingFinished: {
                            if (vm && vm.overrideJavaLocation) {
                                vm.setJavaPath(vm.instanceId, text);
                            }
                        }
                    }
                    Button {
                        text: qsTr("Browse")
                        onClicked: {
                            if (ProjT.launcherVM && vm && vm.overrideJavaLocation) {
                                var path = ProjT.launcherVM.browseForFile(qsTr("Select Java Executable"), qsTr("Java Executable (*.exe *java*);;All Files (*)"));
                                if (path.length > 0) {
                                    vm.setJavaPath(vm.instanceId, path);
                                }
                            }
                        }
                    }
                    Button {
                        text: qsTr("Auto-detect")
                        onClicked: {
                            if (vm && vm.overrideJavaLocation && vm.autoDetectJava) {
                                vm.autoDetectJava(vm.instanceId);
                            }
                        }
                        ToolTip.text: qsTr("Automatically detect installed Java versions")
                        ToolTip.visible: hovered
                    }
                }
                CheckBox {
                    text: qsTr("Override Java location")
                    checked: vm ? vm.overrideJavaLocation : false
                    onToggled: vm ? vm.setOverrideJavaLocation(vm.instanceId, checked) : undefined
                }
                Label {
                    text: qsTr("Memory")
                    color: ThemeColors.text
                    font.bold: true
                }
                RowLayout {
                    spacing: 8
                    Label {
                        text: qsTr("Min (MiB)")
                        color: ThemeColors.textSecondary
                    }
                    SpinBox {
                        id: minMemJava
                        from: 0
                        to: 16384
                        value: vm ? vm.minMemory : 0
                    }
                    Label {
                        text: qsTr("Max (MiB)")
                        color: ThemeColors.textSecondary
                    }
                    SpinBox {
                        id: maxMemJava
                        from: 0
                        to: 32768
                        value: vm ? vm.maxMemory : 0
                    }
                    Button {
                        text: qsTr("Apply")
                        onClicked: vm ? vm.setMemorySettings(vm.instanceId, minMemJava.value, maxMemJava.value) : undefined
                    }
                }
                Label {
                    text: qsTr("JVM Arguments")
                    color: ThemeColors.text
                    font.bold: true
                }
                TextArea {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    wrapMode: Text.Wrap
                    placeholderText: qsTr("-Xmx2G ...")
                    text: vm ? vm.jvmArgs : ""
                    onTextChanged: vm ? vm.setJVMArguments(vm.instanceId, text) : undefined
                }
            }
        }
    }

    Component {
        id: memoryCategory
        Flickable {
            contentWidth: width
            contentHeight: 240
            clip: true
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 4
                spacing: 10
                Label {
                    text: qsTr("Memory")
                    color: ThemeColors.text
                    font.bold: true
                }
                RowLayout {
                    spacing: 8
                    Label {
                        text: qsTr("Min (MiB)")
                        color: ThemeColors.textSecondary
                    }
                    SpinBox {
                        id: memMin
                        from: 0
                        to: 16384
                        value: vm ? vm.minMemory : 0
                    }
                    Label {
                        text: qsTr("Max (MiB)")
                        color: ThemeColors.textSecondary
                    }
                    SpinBox {
                        id: memMax
                        from: 0
                        to: 32768
                        value: vm ? vm.maxMemory : 0
                    }
                    Button {
                        text: qsTr("Apply memory")
                        onClicked: vm ? vm.setMemorySettings(vm.instanceId, memMin.value, memMax.value) : undefined
                    }
                }
                CheckBox {
                    text: qsTr("Override memory settings")
                    checked: vm ? vm.overrideMemory : false
                    onToggled: vm ? vm.setOverrideMemory(vm.instanceId, checked) : undefined
                }
            }
        }
    }

    Component {
        id: argsCategory
        Flickable {
            contentWidth: width
            contentHeight: 220
            clip: true
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 4
                spacing: 10
                Label {
                    text: qsTr("JVM Arguments")
                    color: ThemeColors.text
                    font.bold: true
                }
                TextArea {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    wrapMode: Text.Wrap
                    placeholderText: qsTr("-Xmx2G ...")
                    text: vm ? vm.jvmArgs : ""
                    onTextChanged: vm ? vm.setJVMArguments(vm.instanceId, text) : undefined
                }
            }
        }
    }

    Component {
        id: commandsCategory
        Flickable {
            contentWidth: width
            contentHeight: 220
            clip: true
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 4
                spacing: 10
                Label {
                    text: qsTr("Custom Commands")
                    color: ThemeColors.text
                    font.bold: true
                }
                TextField {
                    Layout.fillWidth: true
                    placeholderText: qsTr("Pre-launch command")
                    text: vm ? vm.preLaunchCommand : ""
                    onEditingFinished: vm ? vm.setPreLaunchCommand(vm.instanceId, text) : undefined
                }
                TextField {
                    Layout.fillWidth: true
                    placeholderText: qsTr("Post-exit command")
                    text: vm ? vm.postExitCommand : ""
                    onEditingFinished: vm ? vm.setPostExitCommand(vm.instanceId, text) : undefined
                }
            }
        }
    }

    Component {
        id: envCategory
        Flickable {
            contentWidth: width
            contentHeight: 260
            clip: true
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 4
                spacing: 8
                Label {
                    text: qsTr("Environment Variables")
                    color: ThemeColors.text
                    font.bold: true
                }
                ListView {
                    id: envList
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    model: vm ? vm.environmentKeys(vm.instanceId) : []
                    delegate: Rectangle {
                        width: envList.width
                        height: 36
                        color: ThemeColors.surface
                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 6
                            spacing: 6
                            Label {
                                text: modelData
                                color: ThemeColors.text
                                Layout.fillWidth: true
                            }
                            Label {
                                text: vm ? vm.environmentValue(vm.instanceId, modelData) : ""
                                color: ThemeColors.textSecondary
                                Layout.fillWidth: true
                            }
                            Button {
                                text: qsTr("Remove")
                                onClicked: vm ? vm.removeEnvironmentVar(vm.instanceId, modelData) : undefined
                            }
                        }
                    }
                }
                RowLayout {
                    spacing: 8
                    TextField {
                        id: envKey
                        placeholderText: qsTr("KEY")
                        Layout.fillWidth: true
                    }
                    TextField {
                        id: envVal
                        placeholderText: qsTr("VALUE")
                        Layout.fillWidth: true
                    }
                    Button {
                        text: qsTr("Add/Update")
                        onClicked: {
                            if (vm && envKey.text.length) {
                                vm.setEnvironmentVar(vm.instanceId, envKey.text, envVal.text);
                                envKey.text = "";
                                envVal.text = "";
                            }
                        }
                    }
                }
                CheckBox {
                    text: qsTr("Override environment")
                    checked: vm ? vm.overrideEnv : false
                    onToggled: vm ? vm.setOverrideEnv(vm.instanceId, checked) : undefined
                }
            }
        }
    }

    Component {
        id: loaderCategory
        Flickable {
            contentWidth: width
            contentHeight: 220
            clip: true
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 4
                spacing: 10
                Label {
                    text: qsTr("Loader")
                    color: ThemeColors.text
                    font.bold: true
                }
                RowLayout {
                    spacing: 8
                    ComboBox {
                        id: loaderTypeCombo
                        model: vm ? vm.availableLoaderTypes : []
                        Layout.fillWidth: true
                        currentIndex: vm && vm.availableLoaderTypes ? vm.availableLoaderTypes.indexOf(vm.loaderType) : -1
                        onActivated: vm ? vm.setLoaderType(vm.instanceId, currentText) : undefined
                    }
                    Button {
                        text: qsTr("Refresh")
                        onClicked: vm ? vm.refreshLoaderVersions(vm.instanceId) : undefined
                    }
                }
                RowLayout {
                    spacing: 8
                    ComboBox {
                        id: loaderVersionCombo
                        model: vm ? vm.availableLoaderVersions : []
                        Layout.fillWidth: true
                        currentIndex: vm && vm.availableLoaderVersions ? vm.availableLoaderVersions.indexOf(vm.loaderVersion) : -1
                        editable: true
                        onActivated: vm ? vm.setLoaderVersion(vm.instanceId, currentText) : undefined
                    }
                    Button {
                        text: qsTr("Apply")
                        onClicked: vm ? vm.setLoaderVersion(vm.instanceId, loaderVersionCombo.editText || loaderVersionCombo.currentText) : undefined
                    }
                }
                CheckBox {
                    text: qsTr("Override loader settings")
                    checked: vm ? vm.overrideLoader : false
                    onToggled: vm ? vm.setOverrideLoader(vm.instanceId, checked) : undefined
                }
            }
        }
    }

    Component {
        id: gameCategory
        Flickable {
            contentWidth: width
            contentHeight: 260
            clip: true
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 4
                spacing: 10
                Label {
                    text: qsTr("Game Settings")
                    color: ThemeColors.text
                    font.bold: true
                }
                TextArea {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    wrapMode: Text.Wrap
                    placeholderText: qsTr("--fullscreen --width 1280 --height 720")
                    text: vm ? vm.gameArgs : ""
                    onTextChanged: vm ? vm.setGameArgs(vm.instanceId, text) : undefined
                }
                RowLayout {
                    spacing: 8
                    CheckBox {
                        text: qsTr("Fullscreen")
                        checked: vm ? vm.fullscreen : false
                        onToggled: vm ? vm.setFullscreen(vm.instanceId, checked) : undefined
                    }
                    Label {
                        text: qsTr("Width")
                        color: ThemeColors.textSecondary
                    }
                    SpinBox {
                        id: gameW
                        from: 640
                        to: 3840
                        value: vm ? vm.resolutionWidth : 0
                    }
                    Label {
                        text: qsTr("Height")
                        color: ThemeColors.textSecondary
                    }
                    SpinBox {
                        id: gameH
                        from: 480
                        to: 2160
                        value: vm ? vm.resolutionHeight : 0
                    }
                    Button {
                        text: qsTr("Apply size")
                        onClicked: vm ? vm.setResolution(vm.instanceId, gameW.value, gameH.value) : undefined
                    }
                }
                RowLayout {
                    spacing: 8
                    CheckBox {
                        text: qsTr("Override game directory")
                        checked: vm ? vm.overrideGameDir : false
                        onToggled: vm ? vm.setOverrideGameDir(vm.instanceId, checked) : undefined
                    }
                    TextField {
                        id: gameDirField
                        Layout.fillWidth: true
                        placeholderText: qsTr("Custom game directory")
                        text: vm ? vm.customGameDir : ""
                        onEditingFinished: vm ? vm.setCustomGameDir(vm.instanceId, text) : undefined
                    }
                    Button {
                        text: qsTr("Browse")
                        enabled: vm && vm.overrideGameDir
                        onClicked: {
                            if (ProjT.launcherVM) {
                                var path = ProjT.launcherVM.browseForDirectory(qsTr("Select Game Directory"));
                                if (path.length > 0) {
                                    if (vm)
                                        vm.setCustomGameDir(vm.instanceId, path);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Component {
        id: notesCategory
        Flickable {
            contentWidth: width
            contentHeight: 220
            clip: true
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 4
                spacing: 10
                Label {
                    text: qsTr("Notes")
                    color: ThemeColors.text
                    font.bold: true
                }
                TextArea {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    wrapMode: Text.Wrap
                    placeholderText: qsTr("Notes for this instance")
                    text: vm ? vm.notes : ""
                    onTextChanged: vm ? vm.setNotes(vm.instanceId, text) : undefined
                }
            }
        }
    }

    Component {
        id: iconCategory
        Flickable {
            contentWidth: width
            contentHeight: 180
            clip: true
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 4
                spacing: 10
                Label {
                    text: qsTr("Icon")
                    color: ThemeColors.text
                    font.bold: true
                }
                ComboBox {
                    id: iconCombo
                    model: vm ? vm.availableIcons : []
                    Layout.fillWidth: true
                    currentIndex: vm && vm.availableIcons ? vm.availableIcons.indexOf(vm.iconKey) : -1
                    onActivated: vm ? vm.setIconKey(vm.instanceId, currentText) : undefined
                }
            }
        }
    }

    Component {
        id: overridesCategory
        Flickable {
            contentWidth: width
            contentHeight: 220
            clip: true
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 4
                spacing: 10
                Label {
                    text: qsTr("Overrides")
                    color: ThemeColors.text
                    font.bold: true
                }
                CheckBox {
                    text: qsTr("Override Java")
                    checked: vm ? vm.overrideJavaLocation : false
                    onToggled: vm ? vm.setOverrideJavaLocation(vm.instanceId, checked) : undefined
                }
                CheckBox {
                    text: qsTr("Override Memory")
                    checked: vm ? vm.overrideMemory : false
                    onToggled: vm ? vm.setOverrideMemory(vm.instanceId, checked) : undefined
                }
                CheckBox {
                    text: qsTr("Override Loader")
                    checked: vm ? vm.overrideLoader : false
                    onToggled: vm ? vm.setOverrideLoader(vm.instanceId, checked) : undefined
                }
                CheckBox {
                    text: qsTr("Override Environment")
                    checked: vm ? vm.overrideEnv : false
                    onToggled: vm ? vm.setOverrideEnv(vm.instanceId, checked) : undefined
                }
            }
        }
    }
}
