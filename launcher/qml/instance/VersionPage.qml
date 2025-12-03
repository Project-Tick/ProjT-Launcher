// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme.js" as Theme

Rectangle {
    id: versionPage
    color: Theme.background
    
    property var vm: ProjT.instanceVM
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingM
        spacing: Theme.spacingS
        
        // Header
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingS
            
            Label {
                text: qsTr("Version")
                font.pointSize: 14
                font.bold: true
                color: Theme.textPrimary
            }
            
            Item { Layout.fillWidth: true }
            
            Button {
                text: qsTr("Add Component")
                icon.name: "list-add"
                onClicked: addComponentDialog.open()
            }
            
            Button {
                text: qsTr("Refresh")
                icon.name: "view-refresh"
                onClicked: {
                    if (vm) vm.refreshVersionComponents()
                }
            }
        }
        
        // Current version info
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Instance Version")
            
            RowLayout {
                anchors.fill: parent
                spacing: Theme.spacingM
                
                Image {
                    Layout.preferredWidth: 48
                    Layout.preferredHeight: 48
                    source: "qrc:/icons/multimc/scalable/instances/minecraft.svg"
                    fillMode: Image.PreserveAspectFit
                }
                
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 4
                    
                    Label {
                        text: vm ? vm.minecraftVersion : ""
                        font.pointSize: 16
                        font.bold: true
                        color: Theme.textPrimary
                    }
                    
                    Label {
                        text: vm ? (vm.modLoaderName ? vm.modLoaderName + " " + vm.modLoaderVersion : qsTr("Vanilla")) : ""
                        color: Theme.textSecondary
                    }
                }
            }
        }
        
        // Components list
        Frame {
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            ListView {
                id: componentsList
                anchors.fill: parent
                clip: true
                model: vm ? vm.componentsModel : []
                
                delegate: ItemDelegate {
                    width: componentsList.width
                    height: 48
                    
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: Theme.spacingS
                        spacing: Theme.spacingS
                        
                        CheckBox {
                            visible: model.canToggle || false
                            checked: model.enabled !== false
                            onCheckedChanged: {
                                if (vm) vm.setComponentEnabled(index, checked)
                            }
                        }
                        
                        Rectangle {
                            Layout.preferredWidth: 32
                            Layout.preferredHeight: 32
                            color: Theme.surfaceVariant
                            radius: 4
                            
                            Label {
                                anchors.centerIn: parent
                                text: {
                                    var name = model.name || ""
                                    if (name.toLowerCase().includes("minecraft")) return "🎮"
                                    if (name.toLowerCase().includes("forge")) return "🔨"
                                    if (name.toLowerCase().includes("fabric")) return "🧵"
                                    if (name.toLowerCase().includes("quilt")) return "🪡"
                                    if (name.toLowerCase().includes("neoforge")) return "⚡"
                                    return "📦"
                                }
                                font.pointSize: 14
                            }
                        }
                        
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2
                            
                            Label {
                                text: model.name || ""
                                color: Theme.textPrimary
                                font.bold: true
                            }
                            
                            Label {
                                text: model.version || ""
                                color: Theme.textSecondary
                                font.pointSize: 9
                            }
                        }
                        
                        Label {
                            text: model.required ? qsTr("Required") : ""
                            color: Theme.accent
                            font.pointSize: 9
                        }
                        
                        ToolButton {
                            icon.name: "go-up"
                            visible: model.canMoveUp || false
                            onClicked: {
                                if (vm) vm.moveComponentUp(index)
                            }
                        }
                        
                        ToolButton {
                            icon.name: "go-down"
                            visible: model.canMoveDown || false
                            onClicked: {
                                if (vm) vm.moveComponentDown(index)
                            }
                        }
                        
                        ToolButton {
                            icon.name: "edit-delete"
                            visible: model.canRemove || false
                            onClicked: {
                                if (vm) vm.removeComponent(index)
                            }
                        }
                    }
                }
                
                ScrollBar.vertical: ScrollBar {}
            }
        }
        
        // Actions
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingS
            
            Button {
                text: qsTr("Change Minecraft Version")
                onClicked: changeVersionDialog.open()
            }
            
            Button {
                text: qsTr("Install Mod Loader")
                onClicked: installLoaderDialog.open()
            }
            
            Item { Layout.fillWidth: true }
            
            Button {
                text: qsTr("Revert to Vanilla")
                enabled: vm && vm.hasModLoader
                onClicked: {
                    if (vm) vm.revertToVanilla()
                }
            }
        }
    }
    
    // Add component dialog
    Dialog {
        id: addComponentDialog
        title: qsTr("Add Component")
        modal: true
        standardButtons: Dialog.Cancel
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        width: 400
        
        ColumnLayout {
            anchors.fill: parent
            spacing: Theme.spacingM
            
            Label {
                text: qsTr("Select a component type to add:")
                color: Theme.textPrimary
            }
            
            Button {
                text: qsTr("Forge")
                Layout.fillWidth: true
                onClicked: {
                    addComponentDialog.close()
                    if (vm) vm.installForge()
                }
            }
            
            Button {
                text: qsTr("Fabric")
                Layout.fillWidth: true
                onClicked: {
                    addComponentDialog.close()
                    if (vm) vm.installFabric()
                }
            }
            
            Button {
                text: qsTr("Quilt")
                Layout.fillWidth: true
                onClicked: {
                    addComponentDialog.close()
                    if (vm) vm.installQuilt()
                }
            }
            
            Button {
                text: qsTr("NeoForge")
                Layout.fillWidth: true
                onClicked: {
                    addComponentDialog.close()
                    if (vm) vm.installNeoForge()
                }
            }
            
            Button {
                text: qsTr("LiteLoader")
                Layout.fillWidth: true
                onClicked: {
                    addComponentDialog.close()
                    if (vm) vm.installLiteLoader()
                }
            }
        }
    }
    
    // Change version dialog
    Dialog {
        id: changeVersionDialog
        title: qsTr("Change Minecraft Version")
        modal: true
        standardButtons: Dialog.Ok | Dialog.Cancel
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        width: 500
        height: 400
        
        ColumnLayout {
            anchors.fill: parent
            spacing: Theme.spacingS
            
            RowLayout {
                Layout.fillWidth: true
                
                CheckBox {
                    text: qsTr("Releases")
                    checked: true
                }
                CheckBox {
                    text: qsTr("Snapshots")
                }
                CheckBox {
                    text: qsTr("Old versions")
                }
            }
            
            ListView {
                id: versionsList
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                model: vm ? vm.availableMinecraftVersions : []
                
                delegate: ItemDelegate {
                    width: versionsList.width
                    height: 32
                    highlighted: ListView.isCurrentItem
                    text: modelData
                    onClicked: versionsList.currentIndex = index
                }
                
                ScrollBar.vertical: ScrollBar {}
            }
        }
        
        onAccepted: {
            if (vm && versionsList.currentIndex >= 0) {
                var version = vm.availableMinecraftVersions[versionsList.currentIndex]
                vm.changeMinecraftVersion(version)
            }
        }
    }
    
    // Install loader dialog
    Dialog {
        id: installLoaderDialog
        title: qsTr("Install Mod Loader")
        modal: true
        standardButtons: Dialog.Cancel
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        width: 400
        
        ColumnLayout {
            anchors.fill: parent
            spacing: Theme.spacingM
            
            Label {
                text: qsTr("Select a mod loader to install:")
                color: Theme.textPrimary
            }
            
            Repeater {
                model: ["Forge", "Fabric", "Quilt", "NeoForge", "LiteLoader"]
                
                Button {
                    text: modelData
                    Layout.fillWidth: true
                    onClicked: {
                        installLoaderDialog.close()
                        if (vm) vm.installModLoader(modelData.toLowerCase())
                    }
                }
            }
        }
    }
}
