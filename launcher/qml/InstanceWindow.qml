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

Item {
    id: root
    width: 840
    height: 560
    visible: true
    property var vm: ProjT.settingsVM

    property var categoriesModel: [
        { key: "java", title: qsTr("Java") },
        { key: "memory", title: qsTr("Memory") },
        { key: "args", title: qsTr("Args") },
        { key: "commands", title: qsTr("Commands") },
        { key: "env", title: qsTr("Environment") },
        { key: "loader", title: qsTr("Loader") },
        { key: "game", title: qsTr("Game") },
        { key: "notes", title: qsTr("Notes") },
        { key: "overrides", title: qsTr("Overrides") }
    ]

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 10

        RowLayout {
            Layout.fillWidth: true
            spacing: 8
            Label {
                text: qsTr("Instance Settings")
                color: "#e0e0e0"
                font.pixelSize: 18
                font.bold: true
            }
            Label {
                text: vm ? vm.instanceId : ""
                color: "#b0bec5"
                font.pixelSize: 12
            }
            Rectangle { Layout.fillWidth: true; color: "transparent" }
            Button {
                text: qsTr("Apply")
                enabled: vm && !vm.busy
                onClicked: vm ? vm.applyChanges() : undefined
            }
            Button {
                text: qsTr("Reset")
                enabled: vm && !vm.busy
                onClicked: vm ? vm.resetChanges() : undefined
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 12

            ListView {
                id: categoryList
                width: 180
                Layout.preferredWidth: 180
                Layout.fillHeight: true
                clip: true
                spacing: 6
                model: categoriesModel
                delegate: Button {
                    id: catButton
                    text: modelData.title
                    checkable: true
                    checked: vm && vm.currentCategory === modelData.key
                    implicitHeight: 36
                    Layout.fillWidth: true
                    background: Rectangle {
                        radius: 6
                        color: catButton.checked ? "#2c3440" : (catButton.hovered ? "#262a31" : "transparent")
                        border.color: catButton.checked ? "#4c93ff" : "#323742"
                        border.width: catButton.checked ? 1 : 0
                    }
                    contentItem: Text {
                        text: catButton.text
                        anchors.centerIn: parent
                        color: catButton.checked ? "#e6f0ff" : "#e0e0e0"
                        font.pointSize: 12
                    }
                    onClicked: {
                        if (vm) {
                            vm.loadCategory(modelData.key)
                        }
                    }
                }
            }

            Frame {
                Layout.fillWidth: true
                Layout.fillHeight: true
                padding: 10

                StackLayout {
                    id: pages
                    anchors.fill: parent
                    currentIndex: {
                        if (!vm) return 0
                        const idx = categoriesModel.findIndex(function(item) { return item.key === vm.currentCategory })
                        return idx >= 0 ? idx : 0
                    }

                    Repeater {
                        model: categoriesModel.length
                        delegate: Loader {
                            property string categoryName: categoriesModel[index].key
                            sourceComponent: categoryComponent(categoryName)
                        }
                    }
                }
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "#000000"
        opacity: vm && vm.busy ? 0.25 : 0
        visible: opacity > 0
        Behavior on opacity { NumberAnimation { duration: 150 } }
        Column {
            anchors.centerIn: parent
            spacing: 6
            BusyIndicator {
                running: vm ? vm.busy : false
                visible: running
            }
            Label {
                text: vm && vm.busyReason ? vm.busyReason : ""
                color: "#e0e0e0"
                visible: text.length > 0
            }
        }
    }

    function categoryComponent(name) {
        switch (name) {
        case "java":
            return javaCategory
        case "memory":
            return memoryCategory
        case "args":
            return argsCategory
        case "commands":
            return commandsCategory
        case "env":
            return envCategory
        default:
            return placeholderCategory
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
                    color: "#e0e0e0"
                }
            }
        }
    }

    Component {
        id: javaCategory
        Flickable {
            contentWidth: width
            contentHeight: 320
            clip: true
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 4
                spacing: 10
                Label { text: qsTr("Java Runtime"); color: "#e0e0e0"; font.bold: true }
                TextField {
                    Layout.fillWidth: true
                    placeholderText: qsTr("Java path")
                    text: vm ? vm.javaPath : ""
                    onEditingFinished: {
                        if (vm) {
                            vm.setJavaPath(vm.instanceId, text)
                        }
                    }
                }
                RowLayout {
                    spacing: 8
                    CheckBox {
                        text: qsTr("Override Java location")
                        checked: vm ? vm.overrideJavaLocation : false
                        onToggled: vm ? vm.setOverrideJavaLocation(vm.instanceId, checked) : undefined
                    }
                }
                Label { text: qsTr("Memory"); color: "#e0e0e0"; font.bold: true }
                RowLayout {
                    spacing: 8
                    Label { text: qsTr("Min (MiB)"); color: "#b0bec5" }
                    SpinBox {
                        id: minMemJava
                        from: 0
                        to: 16384
                        value: vm ? vm.minMemory : 0
                    }
                    Label { text: qsTr("Max (MiB)"); color: "#b0bec5" }
                    SpinBox {
                        id: maxMemJava
                        from: 0
                        to: 32768
                        value: vm ? vm.maxMemory : 0
                    }
                    Button {
                        text: qsTr("Apply")
                        onClicked: {
                            if (vm) {
                                vm.setMemorySettings(vm.instanceId, minMemJava.value, maxMemJava.value)
                            }
                        }
                    }
                }
                Label { text: qsTr("JVM Arguments"); color: "#e0e0e0"; font.bold: true }
                TextArea {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    wrapMode: Text.Wrap
                    placeholderText: qsTr("-Xmx2G ...")
                    text: vm ? vm.jvmArgs : ""
                    onTextChanged: {
                        if (vm) {
                            vm.setJVMArguments(vm.instanceId, text)
                        }
                    }
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
                spacing: 12
                Label { text: qsTr("Memory"); color: "#e0e0e0"; font.bold: true }
                RowLayout {
                    spacing: 8
                    Label { text: qsTr("Min (MiB)"); color: "#b0bec5" }
                    SpinBox {
                        id: minMem
                        from: 0
                        to: 16384
                        value: vm ? vm.minMemory : 0
                    }
                    Label { text: qsTr("Max (MiB)"); color: "#b0bec5" }
                    SpinBox {
                        id: maxMem
                        from: 0
                        to: 32768
                        value: vm ? vm.maxMemory : 0
                    }
                    Button {
                        text: qsTr("Apply memory")
                        onClicked: {
                            if (vm) {
                                vm.setMemorySettings(vm.instanceId, minMem.value, maxMem.value)
                            }
                        }
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
            contentHeight: 260
            clip: true
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 4
                spacing: 10
                Label { text: qsTr("JVM Arguments"); color: "#e0e0e0"; font.bold: true }
                TextArea {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    wrapMode: Text.Wrap
                    placeholderText: qsTr("-Xmx2G ...")
                    text: vm ? vm.jvmArgs : ""
                    onTextChanged: {
                        if (vm) {
                            vm.setJVMArguments(vm.instanceId, text)
                        }
                    }
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
                Label { text: qsTr("Custom Commands"); color: "#e0e0e0"; font.bold: true }
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
                Label { text: qsTr("Environment Variables"); color: "#e0e0e0"; font.bold: true }
                ListView {
                    id: envList
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    model: vm ? vm.environmentKeys(vm.instanceId) : []
                    delegate: Rectangle {
                        width: envList.width
                        height: 36
                        color: "#23262b"
                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 6
                            spacing: 6
                            Label { text: modelData; color: "#e0e0e0"; Layout.fillWidth: true }
                            Label { text: vm ? vm.environmentValue(vm.instanceId, modelData) : ""; color: "#b0bec5"; Layout.fillWidth: true }
                            Button {
                                text: qsTr("Remove")
                                onClicked: vm ? vm.removeEnvironmentVar(vm.instanceId, modelData) : undefined
                            }
                        }
                    }
                }
                RowLayout {
                    spacing: 8
                    TextField { id: envKey; placeholderText: qsTr("KEY"); Layout.fillWidth: true }
                    TextField { id: envVal; placeholderText: qsTr("VALUE"); Layout.fillWidth: true }
                    Button {
                        text: qsTr("Add/Update")
                        onClicked: {
                            if (vm && envKey.text.length) {
                                vm.setEnvironmentVar(vm.instanceId, envKey.text, envVal.text)
                                envKey.text = ""
                                envVal.text = ""
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
                Label { text: qsTr("Loader"); color: "#e0e0e0"; font.bold: true }
                RowLayout {
                    spacing: 8
                    TextField {
                        id: loaderTypeField
                        placeholderText: qsTr("Loader type (Fabric/Forge/Quilt/NeoForge)")
                        Layout.fillWidth: true
                        text: vm ? vm.loaderType : ""
                    }
                    Button {
                        text: qsTr("Set type")
                        onClicked: vm ? vm.setLoaderType(vm.instanceId, loaderTypeField.text) : undefined
                    }
                }
                RowLayout {
                    spacing: 8
                    TextField {
                        id: loaderVersionField
                        placeholderText: qsTr("Loader version")
                        Layout.fillWidth: true
                        text: vm ? vm.loaderVersion : ""
                    }
                    Button {
                        text: qsTr("Set version")
                        onClicked: vm ? vm.setLoaderVersion(vm.instanceId, loaderVersionField.text) : undefined
                    }
                    Button {
                        text: qsTr("Refresh")
                        onClicked: vm ? vm.refreshLoaderVersions(vm.instanceId) : undefined
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
            contentHeight: 220
            clip: true
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 4
                spacing: 10
                Label { text: qsTr("Game Arguments"); color: "#e0e0e0"; font.bold: true }
                TextArea {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    wrapMode: Text.Wrap
                    placeholderText: qsTr("--fullscreen --width 1280 --height 720")
                    text: vm ? vm.gameArgs : ""
                    onTextChanged: {
                        if (vm) {
                            vm.setGameArgs(vm.instanceId, text)
                        }
                    }
                }
                RowLayout {
                    spacing: 8
                    CheckBox {
                        text: qsTr("Fullscreen")
                        checked: vm ? vm.fullscreen : false
                        onToggled: vm ? vm.setFullscreen(vm.instanceId, checked) : undefined
                    }
                    Label { text: qsTr("Width"); color: "#b0bec5" }
                    SpinBox {
                        id: resWidth
                        from: 640; to: 3840
                        value: vm ? vm.resolutionWidth : 0
                    }
                    Label { text: qsTr("Height"); color: "#b0bec5" }
                    SpinBox {
                        id: resHeight
                        from: 480; to: 2160
                        value: vm ? vm.resolutionHeight : 0
                    }
                    Button {
                        text: qsTr("Apply size")
                        onClicked: vm ? vm.setResolution(vm.instanceId, resWidth.value, resHeight.value) : undefined
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
                Label { text: qsTr("Notes"); color: "#e0e0e0"; font.bold: true }
                TextArea {
                    id: notesField
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    wrapMode: Text.Wrap
                    placeholderText: qsTr("Notes for this instance")
                    text: vm ? vm.notes : ""
                    onTextChanged: {
                        if (vm) {
                            vm.setNotes(vm.instanceId, text)
                        }
                    }
                }
                RowLayout {
                    spacing: 8
                    Label { text: qsTr("Icon"); color: "#e0e0e0"; Layout.alignment: Qt.AlignVCenter }
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
                Label { text: qsTr("Overrides"); color: "#e0e0e0"; font.bold: true }
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
