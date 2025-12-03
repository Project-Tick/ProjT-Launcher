// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme.js" as Theme

Dialog {
    id: backupDialog
    title: qsTr("Backup Instance")
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel
    width: 500
    height: 400
    
    property var vm: ProjT.instancesVM
    property string instanceId: ""
    property string instanceName: ""
    
    ColumnLayout {
        anchors.fill: parent
        spacing: Theme.spacingM
        
        Label {
            text: qsTr("Create backup of '%1'").arg(instanceName)
            font.bold: true
            color: Theme.textPrimary
        }
        
        // Backup name
        RowLayout {
            Layout.fillWidth: true
            
            Label {
                text: qsTr("Backup name:")
                color: Theme.textPrimary
            }
            
            TextField {
                id: backupName
                Layout.fillWidth: true
                text: instanceName + "_backup_" + Qt.formatDateTime(new Date(), "yyyyMMdd_HHmmss")
                selectByMouse: true
            }
        }
        
        // Options
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Options")
            
            ColumnLayout {
                anchors.fill: parent
                
                CheckBox {
                    id: includeWorlds
                    text: qsTr("Include worlds/saves")
                    checked: true
                }
                
                CheckBox {
                    id: includeResourcePacks
                    text: qsTr("Include resource packs")
                    checked: true
                }
                
                CheckBox {
                    id: includeShaderPacks
                    text: qsTr("Include shader packs")
                    checked: true
                }
                
                CheckBox {
                    id: includeScreenshots
                    text: qsTr("Include screenshots")
                    checked: false
                }
                
                CheckBox {
                    id: compressBackup
                    text: qsTr("Compress backup (smaller size)")
                    checked: true
                }
            }
        }
        
        // Existing backups
        GroupBox {
            Layout.fillWidth: true
            Layout.fillHeight: true
            title: qsTr("Existing Backups")
            
            ListView {
                id: backupsList
                anchors.fill: parent
                clip: true
                model: vm ? vm.backupsList : []
                
                delegate: ItemDelegate {
                    width: backupsList.width
                    height: 40
                    
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: Theme.spacingS
                        
                        Label {
                            text: modelData.name || modelData
                            color: Theme.textPrimary
                            Layout.fillWidth: true
                        }
                        
                        Label {
                            text: modelData.date || ""
                            color: Theme.textSecondary
                        }
                        
                        Label {
                            text: modelData.size || ""
                            color: Theme.textSecondary
                        }
                        
                        ToolButton {
                            icon.name: "edit-delete"
                            onClicked: {
                                if (vm) vm.deleteBackup(modelData.path)
                            }
                        }
                    }
                }
                
                ScrollBar.vertical: ScrollBar {}
            }
            
            Label {
                anchors.centerIn: parent
                visible: backupsList.count === 0
                text: qsTr("No backups yet")
                color: Theme.textSecondary
            }
        }
    }
    
    onAccepted: {
        if (vm && backupName.text.length > 0) {
            var options = {
                includeWorlds: includeWorlds.checked,
                includeResourcePacks: includeResourcePacks.checked,
                includeShaderPacks: includeShaderPacks.checked,
                includeScreenshots: includeScreenshots.checked,
                compress: compressBackup.checked
            }
            vm.createBackup(instanceId, backupName.text, options)
        }
    }
    
    onOpened: {
        if (vm) vm.loadBackupsList(instanceId)
    }
}
