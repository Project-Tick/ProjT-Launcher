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
import ProjTLauncher 1.0
import "../Theme.js" as Theme

Rectangle {
    id: lowerBar
    color: ThemeColors.toolBar
    height: 32
    
    // Properties from ViewModels
    readonly property var instancesVM: ProjT.instancesVM
    readonly property bool isAnyRunning: instancesVM ? instancesVM.isSelectedRunning : false
    readonly property bool isBusy: instancesVM ? instancesVM.busy : false
    readonly property string busyReason: instancesVM ? instancesVM.busyReason : ""
    
    // Custom status message (can be overridden)
    property string statusMessage: qsTr("Ready")
    
    // Computed display message
    readonly property string displayStatus: {
        if (isBusy && busyReason.length > 0) return busyReason
        if (isAnyRunning) return qsTr("Instance running...")
        return statusMessage
    }
    
    Rectangle {
        anchors.fill: parent
        color: ThemeColors.toolBar
        
        // Top border only
        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            height: 1
            color: ThemeColors.border
        }
        
        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: Theme.spacingM
            anchors.rightMargin: Theme.spacingM
            spacing: Theme.spacingM
            
            // === Status Icon ===
            Rectangle {
                width: 8
                height: 8
                radius: 4
                color: {
                    if (isBusy) return ThemeColors.warning
                    if (isAnyRunning) return ThemeColors.success
                    return ThemeColors.textSecondary
                }
                
                // Pulse animation when busy or running
                SequentialAnimation on opacity {
                    running: isBusy || isAnyRunning
                    loops: Animation.Infinite
                    NumberAnimation { to: 0.4; duration: 800; easing.type: Easing.InOutQuad }
                    NumberAnimation { to: 1.0; duration: 800; easing.type: Easing.InOutQuad }
                }
            }
            
            // === Status Message ===
            Label {
                text: displayStatus
                color: ThemeColors.textSecondary
                font.pointSize: 9
                Layout.fillWidth: true
                elide: Text.ElideRight
            }
            
            // === Progress Bar (when busy) ===
            ProgressBar {
                visible: isBusy
                indeterminate: true
                Layout.preferredWidth: 100
                Layout.preferredHeight: 4
                Layout.alignment: Qt.AlignVCenter
                
                background: Rectangle {
                    color: ThemeColors.backgroundAlt
                    radius: 2
                }
                
                contentItem: Item {
                    Rectangle {
                        width: parent.width * 0.3
                        height: parent.height
                        radius: 2
                        color: ThemeColors.accent
                        
                        SequentialAnimation on x {
                            running: isBusy
                            loops: Animation.Infinite
                            NumberAnimation { 
                                to: parent.width * 0.7
                                duration: 1000
                                easing.type: Easing.InOutQuad 
                            }
                            NumberAnimation { 
                                to: 0
                                duration: 1000
                                easing.type: Easing.InOutQuad 
                            }
                        }
                    }
                }
            }
            
            // === Version Info ===
            Label {
                text: ProjT.launcherVM ? ProjT.launcherVM.versionString : ""
                color: ThemeColors.textSecondary
                font.pointSize: 8
                Layout.alignment: Qt.AlignVCenter
            }
        }
    }
}
