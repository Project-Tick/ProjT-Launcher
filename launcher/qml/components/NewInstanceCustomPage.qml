// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  Custom version selection page for New Instance dialog
 *  Similar to Qt Widget CustomPage
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme.js" as Theme

Rectangle {
    id: customPage
    color: Theme.background
    
    property string selectedVersion: ""
    property var vm: ProjT.instancesVM
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingM
        spacing: Theme.spacingM
        
        // Header
        Label {
            text: qsTr("Custom")
            font.pointSize: 14
            font.bold: true
            color: Theme.textPrimary
        }
        
        Label {
            text: qsTr("Select a Minecraft version to create a new instance")
            color: Theme.textSecondary
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }
        
        // Filter options
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Filter")
            
            RowLayout {
                anchors.fill: parent
                spacing: Theme.spacingM
                
                CheckBox {
                    id: showReleasesCheck
                    text: qsTr("Releases")
                    checked: true
                    onCheckedChanged: filterVersions()
                }
                
                CheckBox {
                    id: showSnapshotsCheck
                    text: qsTr("Snapshots")
                    checked: false
                    onCheckedChanged: filterVersions()
                }
                
                CheckBox {
                    id: showBetasCheck
                    text: qsTr("Betas")
                    checked: false
                    onCheckedChanged: filterVersions()
                }
                
                CheckBox {
                    id: showAlphasCheck
                    text: qsTr("Alphas")
                    checked: false
                    onCheckedChanged: filterVersions()
                }
                
                Item { Layout.fillWidth: true }
                
                Button {
                    text: qsTr("Refresh")
                    icon.name: "view-refresh"
                    onClicked: {
                        if (vm) {
                            vm.refreshVersionList()
                        }
                    }
                }
            }
        }
        
        // Version list
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: Theme.surfaceBackground
            border.color: Theme.border
            border.width: 1
            radius: Theme.radiusS
            
            ListView {
                id: versionListView
                anchors.fill: parent
                anchors.margins: 1
                clip: true
                
                model: vm ? vm.filteredVersionList : []
                
                delegate: ItemDelegate {
                    width: versionListView.width
                    height: 32
                    highlighted: versionListView.currentIndex === index
                    
                    contentItem: RowLayout {
                        spacing: Theme.spacingS
                        
                        Image {
                            Layout.preferredWidth: 16
                            Layout.preferredHeight: 16
                            source: {
                                var type = modelData.type || "release"
                                if (type === "release") return "qrc:/icons/status/grass"
                                if (type === "snapshot") return "qrc:/icons/status/enderpearl"
                                if (type === "beta" || type === "old_beta") return "qrc:/icons/status/bug"
                                if (type === "alpha" || type === "old_alpha") return "qrc:/icons/status/chicken"
                                return "qrc:/icons/status/grass"
                            }
                            fillMode: Image.PreserveAspectFit
                        }
                        
                        Label {
                            text: modelData.version || modelData
                            color: Theme.textPrimary
                            Layout.fillWidth: true
                        }
                        
                        Label {
                            text: modelData.type || ""
                            color: Theme.textSecondary
                            font.pointSize: 9
                        }
                    }
                    
                    onClicked: {
                        versionListView.currentIndex = index
                        customPage.selectedVersion = modelData.version || modelData
                    }
                    
                    onDoubleClicked: {
                        versionListView.currentIndex = index
                        customPage.selectedVersion = modelData.version || modelData
                        // Could trigger OK here
                    }
                }
                
                ScrollBar.vertical: ScrollBar {}
            }
            
            // Empty state
            Label {
                anchors.centerIn: parent
                visible: versionListView.count === 0
                text: qsTr("Loading versions...")
                color: Theme.textSecondary
            }
        }
        
        // Loader selection
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Mod Loader (optional)")
            
            RowLayout {
                anchors.fill: parent
                spacing: Theme.spacingM
                
                ComboBox {
                    id: loaderCombo
                    Layout.preferredWidth: 150
                    model: ["None", "Forge", "Fabric", "Quilt", "NeoForge", "LiteLoader"]
                }
                
                ComboBox {
                    id: loaderVersionCombo
                    Layout.fillWidth: true
                    enabled: loaderCombo.currentIndex > 0
                    model: []
                    
                    Label {
                        anchors.centerIn: parent
                        visible: parent.count === 0 && parent.enabled
                        text: qsTr("Select version first")
                        color: Theme.textSecondary
                    }
                }
            }
        }
    }
    
    function filterVersions() {
        if (vm) {
            vm.setVersionFilter(
                showReleasesCheck.checked,
                showSnapshotsCheck.checked,
                showBetasCheck.checked,
                showAlphasCheck.checked
            )
        }
    }
    
    Component.onCompleted: {
        if (vm) {
            vm.refreshVersionList()
        }
    }
}
