// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  This file is part of ProjT Launcher and is licensed under
 *  the GNU General Public License version 3 or later.
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme.js" as Theme


/**
 * Screenshots Page – Phase 11.C.2
 * Screenshots gallery for instance
 */

Rectangle {
    id: root
    objectName: "screenshotsPage"
    color: Theme.background
    
    property var vm: ProjT.instanceVM
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingM
        spacing: Theme.spacingM
        
        // Header
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingM
            
            Text {
                text: qsTr("Screenshots")
                font.pixelSize: 24
                font.weight: Font.Bold
                color: Theme.foreground
            }
            
            Item { Layout.fillWidth: true }
            
            Button {
                text: qsTr("Open Folder")
                onClicked: if (root.vm) root.vm.openScreenshotsFolder()
                
                background: Rectangle {
                    color: parent.hovered ? Theme.surface1 : Theme.surface0
                    border.color: Theme.surface2
                    border.width: 1
                    radius: Theme.radiusS
                }
                
                contentItem: Text {
                    text: parent.text
                    color: Theme.foreground
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
        
        Text {
            text: root.vm && root.vm.instanceName ? root.vm.instanceName : qsTr("No instance selected")
            font.pixelSize: 14
            color: Theme.mutedForeground
            visible: root.vm !== null
        }
        
        // Toolbar
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingS
            
            Button {
                text: qsTr("Copy")
                enabled: gridView.currentIndex >= 0
                onClicked: {
                    if (root.vm && gridView.currentIndex >= 0) {
                        root.vm.copyScreenshotToClipboard(gridView.currentIndex)
                    }
                }
                
                background: Rectangle {
                    color: parent.enabled ? (parent.hovered ? Theme.surface1 : Theme.surface0) : Theme.mantle
                    border.color: Theme.surface2
                    border.width: 1
                    radius: Theme.radiusS
                }
                
                contentItem: Text {
                    text: parent.text
                    color: parent.enabled ? Theme.foreground : Theme.mutedForeground
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
            
            Button {
                text: qsTr("Delete")
                enabled: gridView.currentIndex >= 0
                onClicked: {
                    if (root.vm && gridView.currentIndex >= 0) {
                        deleteScreenshotDialog.open()
                    }
                }
                
                background: Rectangle {
                    color: parent.enabled ? (parent.hovered ? Theme.red : Theme.surface0) : Theme.mantle
                    border.color: parent.enabled ? Theme.red : Theme.surface2
                    border.width: 1
                    radius: Theme.radiusS
                }
                
                contentItem: Text {
                    text: parent.text
                    color: parent.enabled ? (parent.hovered ? Theme.base : Theme.red) : Theme.mutedForeground
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
            
            Item { Layout.fillWidth: true }
            
            Button {
                text: qsTr("Refresh")
                onClicked: {
                    if (root.vm) root.vm.refreshScreenshots()
                }
                
                background: Rectangle {
                    color: parent.hovered ? Theme.surface1 : Theme.surface0
                    border.color: Theme.surface2
                    border.width: 1
                    radius: Theme.radiusS
                }
                
                contentItem: Text {
                    text: parent.text
                    color: Theme.foreground
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
        
        // Delete Confirmation Dialog
        Dialog {
            id: deleteScreenshotDialog
            title: qsTr("Delete Screenshot")
            modal: true
            standardButtons: Dialog.Yes | Dialog.No
            x: (root.width - width) / 2
            y: (root.height - height) / 2
            
            Label {
                text: qsTr("Are you sure you want to delete this screenshot?")
                wrapMode: Text.WordWrap
            }
            
            onAccepted: {
                if (root.vm && gridView.currentIndex >= 0) {
                    root.vm.deleteScreenshot(gridView.currentIndex)
                }
            }
        }
        
        // Screenshots Grid
        GridView {
            id: gridView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            cellWidth: 210
            cellHeight: 170
            
            // Use ViewModel's screenshot paths
            model: root.vm ? root.vm.screenshotPaths : []
            
            // Empty state
            Text {
                anchors.centerIn: parent
                text: qsTr("No screenshots found.\nTake screenshots in-game (F2) and they will appear here.")
                horizontalAlignment: Text.AlignHCenter
                color: Theme.mutedForeground
                font.pixelSize: 14
                visible: gridView.count === 0
            }
            
            Component.onCompleted: {
                if (root.vm) root.vm.refreshScreenshots()
            }
            
            delegate: Rectangle {
                width: 200
                height: 160
                color: gridView.currentIndex === index ? Theme.selection : Theme.surface0
                border.color: gridView.currentIndex === index ? Theme.accent : Theme.surface1
                border.width: 1
                radius: Theme.radiusS
                
                property string screenshotPath: modelData
                property string screenshotName: root.vm && root.vm.screenshotNames[index] ? root.vm.screenshotNames[index] : ""
                
                MouseArea {
                    anchors.fill: parent
                    onClicked: gridView.currentIndex = index
                    onDoubleClicked: {
                        // Open screenshot in system viewer
                        if (root.vm) root.vm.openScreenshot(index)
                    }
                }
                
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: Theme.spacingS
                    spacing: Theme.spacingXS
                    
                    // Thumbnail
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        color: Theme.mantle
                        radius: Theme.radiusXS
                        
                        Image {
                            anchors.fill: parent
                            anchors.margins: 2
                            source: screenshotPath ? "file://" + screenshotPath : ""
                            fillMode: Image.PreserveAspectCrop
                            asynchronous: true
                            cache: false
                        }
                    }
                    
                    Text {
                        text: screenshotName || qsTr("Screenshot %1").arg(index + 1)
                        color: Theme.foreground
                        font.pixelSize: 11
                        elide: Text.ElideMiddle
                        Layout.fillWidth: true
                    }
                }
            }
        }
    }
}
