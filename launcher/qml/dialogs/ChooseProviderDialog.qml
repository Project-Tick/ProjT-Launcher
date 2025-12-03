// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme.js" as Theme

Dialog {
    id: chooseProviderDialog
    title: qsTr("Choose Download Source")
    modal: true
    standardButtons: Dialog.Cancel
    width: 450
    height: 350
    
    property var providers: []
    property var selectedProvider: null
    property string resourceName: ""
    
    signal providerSelected(var provider)
    
    ColumnLayout {
        anchors.fill: parent
        spacing: Theme.spacingM
        
        // Header
        Label {
            Layout.fillWidth: true
            text: qsTr("Multiple sources are available for \"%1\". Please choose a download source:").arg(resourceName)
            color: Theme.textPrimary
            wrapMode: Text.WordWrap
        }
        
        // Providers list
        Frame {
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            ListView {
                id: providerList
                anchors.fill: parent
                clip: true
                model: providers
                spacing: 4
                
                delegate: ItemDelegate {
                    width: providerList.width
                    height: 64
                    highlighted: selectedProvider && selectedProvider.id === modelData.id
                    
                    Rectangle {
                        anchors.fill: parent
                        radius: 6
                        color: highlighted ? Theme.accent + "20" : "transparent"
                        border.color: highlighted ? Theme.accent : "transparent"
                        border.width: 1
                    }
                    
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: Theme.spacingS
                        spacing: Theme.spacingM
                        
                        // Provider icon
                        Rectangle {
                            Layout.preferredWidth: 44
                            Layout.preferredHeight: 44
                            radius: 8
                            color: {
                                if (modelData.id === "curseforge") return "#f16436"
                                if (modelData.id === "modrinth") return "#1bd96a"
                                return Theme.backgroundAlt
                            }
                            
                            Image {
                                anchors.centerIn: parent
                                width: 28
                                height: 28
                                source: modelData.iconUrl || ""
                                fillMode: Image.PreserveAspectFit
                                visible: status === Image.Ready
                            }
                            
                            Label {
                                anchors.centerIn: parent
                                text: modelData.name ? modelData.name.charAt(0).toUpperCase() : "?"
                                color: "white"
                                font.bold: true
                                font.pointSize: 16
                                visible: parent.children[0].status !== Image.Ready
                            }
                        }
                        
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2
                            
                            Label {
                                text: modelData.name || qsTr("Unknown")
                                color: Theme.textPrimary
                                font.bold: true
                            }
                            
                            Label {
                                text: modelData.description || ""
                                color: Theme.textSecondary
                                font.pointSize: Theme.fontSizeSmall
                                elide: Text.ElideRight
                                Layout.fillWidth: true
                            }
                        }
                        
                        // Preferred badge
                        Rectangle {
                            Layout.preferredWidth: preferredLabel.implicitWidth + 12
                            Layout.preferredHeight: 20
                            radius: 10
                            color: Theme.accent
                            visible: modelData.preferred === true
                            
                            Label {
                                id: preferredLabel
                                anchors.centerIn: parent
                                text: qsTr("Preferred")
                                color: "white"
                                font.pointSize: Theme.fontSizeSmall - 1
                            }
                        }
                    }
                    
                    onClicked: {
                        selectedProvider = modelData
                    }
                    
                    onDoubleClicked: {
                        selectedProvider = modelData
                        providerSelected(modelData)
                        chooseProviderDialog.accept()
                    }
                }
                
                ScrollBar.vertical: ScrollBar {}
            }
        }
        
        // Buttons
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingM
            
            CheckBox {
                id: rememberCheck
                text: qsTr("Remember my choice")
            }
            
            Item { Layout.fillWidth: true }
            
            Button {
                text: qsTr("Select")
                highlighted: true
                enabled: selectedProvider !== null
                onClicked: {
                    if (rememberCheck.checked && ProjT) {
                        ProjT.settings.preferredProvider = selectedProvider.id
                    }
                    providerSelected(selectedProvider)
                    chooseProviderDialog.accept()
                }
            }
        }
    }
}
