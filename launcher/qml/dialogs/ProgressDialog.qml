// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme.js" as Theme

Dialog {
    id: progressDialog
    title: qsTr("Progress")
    modal: true
    closePolicy: Popup.NoAutoClose
    width: 450
    height: 200
    standardButtons: Dialog.Cancel
    
    property string taskName: ""
    property real progress: 0
    property string statusText: ""
    property bool indeterminate: false
    property var vm: null
    
    ColumnLayout {
        anchors.fill: parent
        spacing: Theme.spacingM
        
        Label {
            text: taskName
            font.bold: true
            font.pointSize: 12
            color: Theme.textPrimary
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
        }
        
        ProgressBar {
            id: progressBar
            Layout.fillWidth: true
            value: progress
            indeterminate: progressDialog.indeterminate
        }
        
        Label {
            text: indeterminate ? statusText : qsTr("%1% - %2").arg(Math.round(progress * 100)).arg(statusText)
            color: Theme.textSecondary
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
        }
        
        Item { Layout.fillHeight: true }
    }
    
    onRejected: {
        if (vm) vm.cancelTask()
    }
}
