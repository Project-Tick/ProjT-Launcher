// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * BottomBar Component - Status and News Toolbar
 * 
 * Equivalent to MainWindow's newsToolBar + statusBar in Widgets version
 * Contains: Status message, More News button
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "." as Components
import "../Theme.js" as Theme

Rectangle {
    id: bottomBar
    color: Theme.surface
    height: 40
    width: parent.width
    
    // Properties
    property string statusMessage: qsTr("Ready")
    
    // Signals
    signal moreNewsRequested()
    
    Rectangle {
        anchors.fill: parent
        color: Theme.surface
        border.color: "#323742"
        border.width: 1
        
        RowLayout {
            anchors.fill: parent
            anchors.margins: 0
            spacing: Theme.spacingM
            anchors.leftMargin: Theme.spacingM
            anchors.rightMargin: Theme.spacingM
            
            // === Status Message ===
            Label {
                text: bottomBar.statusMessage
                color: Theme.textSecondary
                font.pointSize: 9
                Layout.fillWidth: true
                
                elide: Text.ElideRight
            }
            
            ToolSeparator {
                orientation: Qt.Vertical
                Layout.fillHeight: true
            }
            
            // === More News ===
            Button {
                text: qsTr("More News")
                icon.name: "document-properties"
                Layout.preferredHeight: 32
                
                onClicked: bottomBar.moreNewsRequested()
                
                ToolTip.text: qsTr("View more news")
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }
        }
    }
}
