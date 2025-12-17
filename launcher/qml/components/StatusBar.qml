// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
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
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import ProjTLauncher 1.0
import "../Theme.js" as Theme

Rectangle {
    id: statusBar
    height: 24

    // Theme binding - directly from themeVM for reliable updates
    property var themeVM: ProjT.themeVM
    property int _themeUpdateCount: 0

    // Computed colors for theme reactivity
    color: {
        var _ = _themeUpdateCount;
        return themeVM ? Qt.darker(themeVM.windowColor, 1.05) : ThemeColors.toolBar;
    }

    property color toolBarColor: {
        var _ = _themeUpdateCount;
        return themeVM ? Qt.darker(themeVM.windowColor, 1.05) : ThemeColors.toolBar;
    }
    property color borderColor: {
        var _ = _themeUpdateCount;
        return themeVM ? Qt.darker(themeVM.windowColor, 1.2) : ThemeColors.border;
    }
    property color textSecondaryColor: {
        var _ = _themeUpdateCount;
        return themeVM ? Qt.darker(themeVM.textColor, 1.3) : ThemeColors.textSecondary;
    }
    property color successColor: ThemeColors.success

    // Listen for theme changes
    Connections {
        target: themeVM
        function onThemeColorsChanged() {
            console.log("[StatusBar] Theme colors changed");
            statusBar._themeUpdateCount++;
        }
    }

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
        if (isBusy && busyReason.length > 0)
            return busyReason;
        if (isRunning)
            return qsTr("Instance running");
        if (statusLeft.length > 0)
            return statusLeft;
        return qsTr("Ready");
    }

    Rectangle {
        anchors.fill: parent
        color: statusBar.toolBarColor

        // Top border only
        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            height: 1
            color: statusBar.borderColor
        }

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: Theme.spacingS
            anchors.rightMargin: Theme.spacingS
            spacing: Theme.spacingM

            // === Status Left (main status message) ===
            Label {
                text: computedStatusLeft
                color: isRunning ? statusBar.successColor : statusBar.textSecondaryColor
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
                color: statusBar.textSecondaryColor
                font.pointSize: 9
                Layout.alignment: Qt.AlignVCenter
            }
        }
    }
}
