/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import ProjTLauncher 1.0
import "../Theme.js" as Theme

Pane {
    id: control
    default property alias content: contentLayout.data
    property string title: ""
    property string icon: ""  // Fallback emoji icon
    property string iconSource: ""  // SVG icon path - takes priority
    property bool collapsible: false
    property bool collapsed: false

    padding: 0
    topPadding: 0
    bottomPadding: 0
    leftPadding: 0
    rightPadding: 0

    background: Rectangle {
        color: ThemeColors.cardBackground
        radius: 12
        border.color: Qt.rgba(ThemeColors.cardBorder.r, ThemeColors.cardBorder.g, ThemeColors.cardBorder.b, 0.5)
        border.width: 1
        
        // Subtle gradient overlay for depth
        Rectangle {
            anchors.fill: parent
            radius: parent.radius
            gradient: Gradient {
                GradientStop { position: 0.0; color: Qt.rgba(255, 255, 255, 0.02) }
                GradientStop { position: 1.0; color: "transparent" }
            }
        }
    }

    ColumnLayout {
        id: mainLayout
        anchors.fill: parent
        spacing: 0

        // Header
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 52
            color: Qt.rgba(ThemeColors.surface.r, ThemeColors.surface.g, ThemeColors.surface.b, 0.3)
            radius: 12
            visible: control.title !== ""

            // Only round top corners
            Rectangle {
                anchors.bottom: parent.bottom
                width: parent.width
                height: parent.radius
                color: parent.color
            }

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 16
                anchors.rightMargin: 16
                spacing: 12

                // Icon container with subtle background
                Rectangle {
                    width: 32
                    height: 32
                    radius: 8
                    color: Qt.rgba(ThemeColors.accent.r, ThemeColors.accent.g, ThemeColors.accent.b, 0.1)
                    visible: control.icon !== "" || control.iconSource !== ""
                    
                    // SVG Icon (priority)
                    Image {
                        anchors.centerIn: parent
                        width: 18
                        height: 18
                        source: control.iconSource
                        visible: control.iconSource !== ""
                        sourceSize: Qt.size(18, 18)
                    }
                    
                    // Fallback emoji icon
                    Text {
                        anchors.centerIn: parent
                        text: control.icon
                        font.pixelSize: 16
                        visible: control.iconSource === "" && control.icon !== ""
                    }
                }

                Text {
                    text: control.title
                    color: ThemeColors.textTitle
                    font.pixelSize: 14
                    font.weight: Font.DemiBold
                    font.letterSpacing: 0.3
                    Layout.fillWidth: true
                }

                // Collapse button
                Rectangle {
                    visible: control.collapsible
                    width: 28
                    height: 28
                    radius: 6
                    color: collapseHover.containsMouse ? Qt.rgba(255, 255, 255, 0.08) : "transparent"
                    
                    Text {
                        anchors.centerIn: parent
                        text: control.collapsed ? "▶" : "▼"
                        color: ThemeColors.textSecondary
                        font.pixelSize: 10
                    }
                    
                    MouseArea {
                        id: collapseHover
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: control.collapsed = !control.collapsed
                    }
                }
            }

            // Divider
            Rectangle {
                anchors.bottom: parent.bottom
                width: parent.width
                height: 1
                color: Qt.rgba(ThemeColors.cardBorder.r, ThemeColors.cardBorder.g, ThemeColors.cardBorder.b, 0.3)
                visible: !control.collapsed
            }
        }

        // Content Container
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredHeight: control.collapsed ? 0 : contentLayout.implicitHeight + 32
            clip: true
            visible: !control.collapsed
            
            Behavior on Layout.preferredHeight {
                NumberAnimation { duration: 200; easing.type: Easing.OutCubic }
            }

            ColumnLayout {
                id: contentLayout
                anchors.fill: parent
                anchors.margins: 16
                spacing: 12
            }
        }
    }
}
