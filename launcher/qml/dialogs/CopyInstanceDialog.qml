// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme.js" as Theme

Dialog {
    id: copyInstanceDialog
    title: qsTr("Copy Instance")
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel
    width: 450
    height: 400
    
    property var sourceInstance: null
    property string newName: ""
    property string newGroup: ""
    property bool copyWorlds: true
    property bool copyResourcePacks: true
    property bool copyShaderPacks: true
    property bool copyMods: true
    property bool copyScreenshots: false
    property bool copyServers: true
    
    ColumnLayout {
        anchors.fill: parent
        spacing: Theme.spacingM
        
        // Source info
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Source Instance")
            
            RowLayout {
                anchors.fill: parent
                spacing: Theme.spacingM
                
                Rectangle {
                    Layout.preferredWidth: 48
                    Layout.preferredHeight: 48
                    radius: 8
                    color: Theme.backgroundAlt
                    
                    Image {
                        anchors.fill: parent
                        anchors.margins: 4
                        source: sourceInstance ? sourceInstance.iconPath : ""
                        fillMode: Image.PreserveAspectFit
                    }
                }
                
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 2
                    
                    Label {
                        text: sourceInstance ? sourceInstance.name : qsTr("No instance selected")
                        color: Theme.textPrimary
                        font.bold: true
                    }
                    
                    Label {
                        text: sourceInstance ? sourceInstance.version : ""
                        color: Theme.textSecondary
                        font.pointSize: Theme.fontSizeSmall
                    }
                }
            }
        }
        
        // New instance settings
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("New Instance")
            
            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS
                
                RowLayout {
                    Layout.fillWidth: true
                    
                    Label {
                        text: qsTr("Name:")
                        Layout.preferredWidth: 80
                    }
                    
                    TextField {
                        id: nameField
                        Layout.fillWidth: true
                        placeholderText: qsTr("Enter instance name...")
                        text: newName || (sourceInstance ? sourceInstance.name + " (Copy)" : "")
                        onTextChanged: newName = text
                    }
                }
                
                RowLayout {
                    Layout.fillWidth: true
                    
                    Label {
                        text: qsTr("Group:")
                        Layout.preferredWidth: 80
                    }
                    
                    ComboBox {
                        id: groupCombo
                        Layout.fillWidth: true
                        editable: true
                        model: ProjT && ProjT.instancesVM ? ProjT.instancesVM.groupList : []
                        editText: newGroup
                        onEditTextChanged: newGroup = editText
                    }
                }
            }
        }
        
        // Copy options
        GroupBox {
            Layout.fillWidth: true
            Layout.fillHeight: true
            title: qsTr("Copy Options")
            
            GridLayout {
                anchors.fill: parent
                columns: 2
                columnSpacing: Theme.spacingM
                rowSpacing: Theme.spacingS
                
                CheckBox {
                    text: qsTr("Worlds/Saves")
                    checked: copyWorlds
                    onCheckedChanged: copyWorlds = checked
                }
                
                CheckBox {
                    text: qsTr("Mods")
                    checked: copyMods
                    onCheckedChanged: copyMods = checked
                }
                
                CheckBox {
                    text: qsTr("Resource Packs")
                    checked: copyResourcePacks
                    onCheckedChanged: copyResourcePacks = checked
                }
                
                CheckBox {
                    text: qsTr("Shader Packs")
                    checked: copyShaderPacks
                    onCheckedChanged: copyShaderPacks = checked
                }
                
                CheckBox {
                    text: qsTr("Servers")
                    checked: copyServers
                    onCheckedChanged: copyServers = checked
                }
                
                CheckBox {
                    text: qsTr("Screenshots")
                    checked: copyScreenshots
                    onCheckedChanged: copyScreenshots = checked
                }
            }
        }
        
        // Info
        Label {
            Layout.fillWidth: true
            text: qsTr("Settings and configuration will always be copied.")
            color: Theme.textSecondary
            font.pointSize: Theme.fontSizeSmall
            font.italic: true
        }
    }
    
    onAccepted: {
        if (sourceInstance && ProjT && ProjT.instancesVM) {
            ProjT.instancesVM.copyInstance(
                sourceInstance.id,
                nameField.text,
                groupCombo.editText,
                {
                    worlds: copyWorlds,
                    mods: copyMods,
                    resourcePacks: copyResourcePacks,
                    shaderPacks: copyShaderPacks,
                    servers: copyServers,
                    screenshots: copyScreenshots
                }
            )
        }
    }
}
