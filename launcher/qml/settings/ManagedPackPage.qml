// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme.js" as Theme

/**
 * Managed Pack Page – Phase 11.C.3
 * Displays information about managed pack
 */

Rectangle {
    objectName: "managedPackPage"
    color: Theme.background
    
    Flickable {
        anchors.fill: parent
        anchors.margins: Theme.spacingM
        contentHeight: column.height
        
        ColumnLayout {
            id: column
            width: parent.width
            spacing: Theme.spacingM
            
            Text {
                text: qsTr("Managed Pack")
                font.pixelSize: 20
                font.weight: Font.Bold
                color: Theme.foreground
            }
            
            GroupBox {
                title: qsTr("Pack Information")
                Layout.fillWidth: true
                
                GridLayout {
                    width: parent.width
                    columns: 2
                    columnSpacing: Theme.spacingM
                    rowSpacing: Theme.spacingS
                    
                    Text { text: qsTr("Name:"); color: Theme.foreground; font.bold: true }
                    Text { text: "My Modpack v1.0"; color: Theme.foreground }
                    
                    Text { text: qsTr("Version:"); color: Theme.foreground; font.bold: true }
                    Text { text: "1.20.1"; color: Theme.foreground }
                    
                    Text { text: qsTr("Author:"); color: Theme.foreground; font.bold: true }
                    Text { text: "Pack Creator"; color: Theme.foreground }
                }
            }
            
            GroupBox {
                title: qsTr("Links")
                Layout.fillWidth: true
                
                ColumnLayout {
                    width: parent.width
                    spacing: Theme.spacingS
                    
                    Button { text: qsTr("View Online"); width: 150 }
                    Button { text: qsTr("Update Pack"); width: 150 }
                    Button { text: qsTr("Report Issue"); width: 150 }
                }
            }
            
            Item { Layout.fillHeight: true }
        }
    }
}
