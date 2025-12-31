// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import ProjTLauncher 1.0
import "../Theme.js" as Theme

Rectangle {
    id: delegate

    // Properties
    property string instanceId: ""
    property string instanceName: ""
    property string instanceGroup: ""
    property string iconPath: ""
    property bool isSelected: false
    property bool isRunning: false
    property string lastPlayedText: ""

    // Signals
    signal clicked(string instanceId)
    signal doubleClicked(string instanceId)
    signal rightClicked(string instanceId, int mouseX, int mouseY)

    width: 140
    height: 180
    color: isSelected ? ThemeColors.cardSelected
                      : (mouseArea.containsMouse ? ThemeColors.cardHover : ThemeColors.cardBackground)
    border.color: isSelected ? ThemeColors.accentEnd : ThemeColors.separator
    border.width: 1
    radius: ThemeColors.radiusM

    // Subtle scale effect on hover
    scale: mouseArea.containsMouse ? 1.02 : 1.0

    Behavior on color {
        ColorAnimation {
            duration: ThemeColors.durationShort
            easing.type: ThemeColors.easeType
        }
    }

    Behavior on scale {
        NumberAnimation {
            duration: ThemeColors.durationShort
            easing.type: ThemeColors.easeType
        }
    }

    Behavior on border.color {
        ColorAnimation {
            duration: ThemeColors.durationShort
            easing.type: ThemeColors.easeType
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: ThemeColors.spacingS
        spacing: ThemeColors.spacingS

        // === Instance Icon ===
        Rectangle {
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: Theme.spacingS
            width: 80
            height: 80
            radius: ThemeColors.radiusS
            color: ThemeColors.backgroundAlt
            border.color: ThemeColors.separator
            border.width: 1

            Image {
                anchors.fill: parent
                anchors.margins: 4
                source: iconPath ? ("file://" + iconPath) : ""
                sourceSize: Qt.size(80, 80)
                fillMode: Image.PreserveAspectFit
                asynchronous: true
                cache: true
                layer.enabled: true
                layer.smooth: true
                smooth: true

                // Fallback text if no icon
                Rectangle {
                    anchors.fill: parent
                    visible: !parent.status || parent.status === Image.Error
                    color: ThemeColors.backgroundAlt
                    radius: ThemeColors.radiusS

                    Text {
                        anchors.centerIn: parent
                        text: instanceName.charAt(0).toUpperCase()
                        font.pointSize: 24
                        font.bold: true
                        color: ThemeColors.text
                    }
                }
            }
            
            // Running indicator overlay
            Rectangle {
                width: 20
                height: 20
                radius: 10
                visible: isRunning
                color: ThemeColors.success
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.margins: -4
                border.color: ThemeColors.cardBackground
                border.width: 2

                Text {
                    anchors.centerIn: parent
                    text: "R"
                    color: ThemeColors.highlightedText
                    font.pointSize: 10
                    font.bold: true
                }
            }
        }

        // === Instance Info ===
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 2
            
            Text {
                text: instanceName
                color: ThemeColors.text
                font.pointSize: 11
                font.bold: true
                elide: Text.ElideRight
                wrapMode: Text.Wrap
                maximumLineCount: 2
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignHCenter
            }

            Text {
                text: instanceGroup ? (instanceGroup) : qsTr("No Group")
                color: ThemeColors.textSecondary
                font.pointSize: 9
                elide: Text.ElideRight
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignHCenter
            }

            Item { Layout.fillHeight: true } // Spacer

            // Play Button (only visible on hover or selection to keep clean)
            ThemedButton {
                text: isRunning ? qsTr("Running...") : qsTr("Play")
                size: "small"
                success: true
                enabled: !isRunning
                Layout.alignment: Qt.AlignHCenter
                visible: mouseArea.containsMouse || isSelected
                opacity: visible ? 1 : 0
                
                onClicked: delegate.doubleClicked(instanceId)
                
                Behavior on opacity { NumberAnimation { duration: 100 } }
            }
        }
    }

    // === Mouse Area for Selection & Context Menu ===
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        propagateComposedEvents: true

        onClicked: function (mouse) {
            if (mouse.button === Qt.LeftButton) {
                delegate.clicked(instanceId);
            } else if (mouse.button === Qt.RightButton) {
                // Map to global screen coordinates for context menu
                var globalPos = mapToGlobal(mouse.x, mouse.y);
                delegate.rightClicked(instanceId, globalPos.x, globalPos.y);
            }
        }

        onDoubleClicked: function (mouse) {
            if (mouse.button === Qt.LeftButton) {
                delegate.doubleClicked(instanceId);
            }
        }
    }

    // === Tooltip ===
    ToolTip.text: instanceName + (instanceGroup ? " (" + instanceGroup + ")" : "")
    ToolTip.visible: mouseArea.containsMouse && !isRunning
    ToolTip.delay: 800
}
