// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme.js" as Theme

Dialog {
    id: newsDialog
    title: qsTr("News")
    modal: true
    width: 600
    height: 500
    standardButtons: Dialog.Close
    
    property var newsItem: null
    
    ColumnLayout {
        anchors.fill: parent
        spacing: Theme.spacingM
        
        // Header image
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 200
            radius: 8
            color: Theme.backgroundAlt
            clip: true
            visible: newsItem && newsItem.imageUrl
            
            Image {
                anchors.fill: parent
                source: newsItem ? newsItem.imageUrl : ""
                fillMode: Image.PreserveAspectCrop
            }
        }
        
        // Title and meta
        ColumnLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingS
            
            Label {
                text: newsItem ? newsItem.title : ""
                color: Theme.textPrimary
                font.bold: true
                font.pointSize: Theme.fontSizeMedium + 2
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }
            
            RowLayout {
                Layout.fillWidth: true
                spacing: Theme.spacingM
                
                Label {
                    text: newsItem ? newsItem.date : ""
                    color: Theme.textSecondary
                    font.pointSize: Theme.fontSizeSmall
                }
                
                Rectangle {
                    Layout.preferredWidth: categoryLabel.implicitWidth + 12
                    Layout.preferredHeight: 20
                    radius: 10
                    color: Theme.accent
                    visible: newsItem && newsItem.category
                    
                    Label {
                        id: categoryLabel
                        anchors.centerIn: parent
                        text: newsItem ? newsItem.category : ""
                        color: "white"
                        font.pointSize: Theme.fontSizeSmall - 1
                    }
                }
            }
        }
        
        // Content
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            
            TextArea {
                readOnly: true
                text: newsItem ? newsItem.content : ""
                wrapMode: Text.WordWrap
                color: Theme.textPrimary
                textFormat: Text.MarkdownText
                background: Rectangle {
                    color: "transparent"
                }
            }
        }
        
        // Actions
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingM
            
            Button {
                text: qsTr("Open in Browser")
                icon.name: "internet-web-browser"
                visible: newsItem && newsItem.url
                onClicked: {
                    if (newsItem && newsItem.url) {
                        Qt.openUrlExternally(newsItem.url)
                    }
                }
            }
            
            Item { Layout.fillWidth: true }
        }
    }
}
