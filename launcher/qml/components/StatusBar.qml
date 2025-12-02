// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  StatusBar - Bottom status bar
 *  
 *  Matches Widget MainWindow statusBar:
 *  [Status Left] [Status Center (version)] [Status Right]
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme.js" as Theme

Rectangle {
    id: statusBar
    color: Theme.surfaceVariant
    height: 24
    
    // Status properties
    property string statusLeft: ""
    property string statusCenter: ProjT.launcherVM ? ProjT.launcherVM.versionString : ""
    property string statusRight: ""
    
    // Instance state
    readonly property var instancesVM: ProjT.instancesVM
    readonly property bool isBusy: instancesVM ? instancesVM.busy : false
    readonly property string busyReason: instancesVM ? instancesVM.busyReason : ""
    readonly property bool isRunning: instancesVM ? instancesVM.isSelectedRunning : false
    
    // Computed left status
    readonly property string computedStatusLeft: {
        if (isBusy && busyReason.length > 0) return busyReason
        if (isRunning) return qsTr("Instance running")
        if (statusLeft.length > 0) return statusLeft
        return qsTr("Ready")
    }
    
    Rectangle {
        anchors.fill: parent
        color: Theme.surfaceVariant
        border.color: "#323742"
        border.width: 1
        
        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: Theme.spacingS
            anchors.rightMargin: Theme.spacingS
            spacing: Theme.spacingM
            
            // === Status Left (main status message) ===
            Label {
                text: computedStatusLeft
                color: isRunning ? "#4ade80" : Theme.textSecondary
                font.pointSize: 9
                elide: Text.ElideRight
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter
            }
            
            // === Busy Indicator ===
            BusyIndicator {
                running: isBusy
                visible: isBusy
                Layout.preferredWidth: 16
                Layout.preferredHeight: 16
                Layout.alignment: Qt.AlignVCenter
            }
            
            // === Status Center (version info) ===
            Label {
                text: statusCenter
                color: Theme.textSecondary
                font.pointSize: 9
                Layout.alignment: Qt.AlignVCenter
            }
        }
    }
}
