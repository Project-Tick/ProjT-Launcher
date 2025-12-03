// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme.js" as Theme

Dialog {
    id: importResourceDialog
    title: qsTr("Import Resource")
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel
    width: 500
    height: 400
    
    property string resourceType: "mod" // mod, resourcepack, shaderpack, world
    property var filePaths: []
    property var vm: null
    
    ColumnLayout {
        anchors.fill: parent
        spacing: Theme.spacingM
        
        // Resource type selector
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingM
            
            Label {
                text: qsTr("Resource Type:")
            }
            
            ComboBox {
                id: typeCombo
                Layout.fillWidth: true
                model: [
                    { text: qsTr("Mod"), value: "mod" },
                    { text: qsTr("Resource Pack"), value: "resourcepack" },
                    { text: qsTr("Shader Pack"), value: "shaderpack" },
                    { text: qsTr("World/Save"), value: "world" },
                    { text: qsTr("Data Pack"), value: "datapack" }
                ]
                textRole: "text"
                valueRole: "value"
                currentIndex: {
                    for (var i = 0; i < model.length; i++) {
                        if (model[i].value === resourceType) return i
                    }
                    return 0
                }
                onCurrentValueChanged: resourceType = currentValue
            }
        }
        
        // File list
        GroupBox {
            Layout.fillWidth: true
            Layout.fillHeight: true
            title: qsTr("Files to Import")
            
            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS
                
                ListView {
                    id: filesList
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    model: filePaths
                    
                    delegate: Rectangle {
                        width: filesList.width
                        height: 40
                        radius: 4
                        color: index % 2 === 0 ? "transparent" : Theme.backgroundAlt
                        
                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: Theme.spacingS
                            spacing: Theme.spacingS
                            
                            Label {
                                text: {
                                    var path = modelData
                                    return path.substring(path.lastIndexOf("/") + 1)
                                }
                                color: Theme.textPrimary
                                Layout.fillWidth: true
                                elide: Text.ElideMiddle
                            }
                            
                            ToolButton {
                                icon.name: "list-remove"
                                onClicked: {
                                    var newPaths = filePaths.slice()
                                    newPaths.splice(index, 1)
                                    filePaths = newPaths
                                }
                            }
                        }
                    }
                    
                    ScrollBar.vertical: ScrollBar {}
                }
                
                Label {
                    Layout.alignment: Qt.AlignCenter
                    text: qsTr("No files selected")
                    color: Theme.textSecondary
                    visible: filePaths.length === 0
                }
                
                // Add files button
                Button {
                    Layout.fillWidth: true
                    text: qsTr("Add Files...")
                    icon.name: "document-open"
                    onClicked: openFileDialog()
                }
            }
        }
        
        // Options
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Options")
            visible: resourceType === "mod"
            
            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS
                
                CheckBox {
                    id: enableCheck
                    text: qsTr("Enable after import")
                    checked: true
                }
                
                CheckBox {
                    id: checkDepsCheck
                    text: qsTr("Check for dependencies")
                    checked: true
                }
            }
        }
        
        // Drop zone hint
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            radius: 8
            color: "transparent"
            border.color: Theme.accent
            border.width: 2
            visible: filePaths.length === 0
            
            Label {
                anchors.centerIn: parent
                text: qsTr("Drag and drop files here or use 'Add Files' button")
                color: Theme.textSecondary
            }
            
            DropArea {
                anchors.fill: parent
                onDropped: {
                    if (drop.hasUrls) {
                        var newPaths = filePaths.slice()
                        for (var i = 0; i < drop.urls.length; i++) {
                            var url = drop.urls[i].toString()
                            // Remove file:// prefix
                            if (url.startsWith("file://")) {
                                url = url.substring(7)
                            }
                            if (newPaths.indexOf(url) === -1) {
                                newPaths.push(url)
                            }
                        }
                        filePaths = newPaths
                    }
                }
            }
        }
        
        // Summary
        Label {
            Layout.fillWidth: true
            text: qsTr("%1 file(s) selected for import").arg(filePaths.length)
            color: Theme.textSecondary
            font.pointSize: Theme.fontSizeSmall
        }
    }
    
    // Use ViewModel to open file dialog (Qt 6 compatible)
    function openFileDialog() {
        if (ProjT && ProjT.launcherVM) {
            var filter = ""
            switch (resourceType) {
                case "mod": filter = "Mod files (*.jar *.zip)"; break
                case "resourcepack": filter = "Resource packs (*.zip)"; break
                case "shaderpack": filter = "Shader packs (*.zip)"; break
                case "world": filter = "World saves (*.zip)"; break
                case "datapack": filter = "Data packs (*.zip)"; break
                default: filter = "All files (*)"; break
            }
            var files = ProjT.launcherVM.browseForFiles(qsTr("Select Files to Import"), filter)
            if (files && files.length > 0) {
                var newPaths = filePaths.slice()
                for (var i = 0; i < files.length; i++) {
                    if (newPaths.indexOf(files[i]) === -1) {
                        newPaths.push(files[i])
                    }
                }
                filePaths = newPaths
            }
        }
    }
    
    onAccepted: {
        if (vm && filePaths.length > 0) {
            vm.importResources(
                resourceType,
                filePaths,
                {
                    enable: enableCheck.checked,
                    checkDependencies: checkDepsCheck.checked
                }
            )
        }
    }
}
