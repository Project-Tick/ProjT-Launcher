// SPDX-License-Identifier: GPL-3.0-only
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
 *
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import ProjTLauncher 1.0
import "../Theme.js" as Theme

Menu {
    id: contextMenu

    // Properties
    property string instanceId: ""
    property bool canLaunch: true
    property bool isRunning: false

    // Signals
    signal launch
    signal editSettings
    signal rename
    signal duplicate
    signal openFolder
    signal backup
    signal exportInstance
    signal deleteInstance
    signal createNew
    signal importInstance

    // Styling
    background: Rectangle {
        color: ThemeColors.surface
        border.color: ThemeColors.border
        border.width: 1
        radius: Theme.radius
    }

    contentItem: ListView {
        implicitWidth: 200
        implicitHeight: contentHeight
        model: contextMenu.contentModel
        ScrollIndicator.vertical: ScrollIndicator {}
    }

    // === Launch ===
    MenuItem {
        text: contextMenu.isRunning ? qsTr("Kill") : qsTr("Launch")
        icon.name: contextMenu.isRunning ? "process-stop" : "media-playback-start"
        enabled: contextMenu.canLaunch
        onTriggered: contextMenu.launch()
    }

    MenuSeparator {}

    // === Create New / Import ===
    MenuItem {
        text: qsTr("Create New Instance")
        icon.name: "document-new"
        onTriggered: contextMenu.createNew()
    }

    MenuItem {
        text: qsTr("Import Instance")
        icon.name: "document-open"
        onTriggered: contextMenu.importInstance()
    }

    MenuSeparator {}

    // === Edit ===
    MenuItem {
        text: qsTr("Settings...")
        icon.name: "preferences-system"
        onTriggered: contextMenu.editSettings()
    }

    MenuItem {
        text: qsTr("Rename...")
        icon.name: "edit-rename"
        onTriggered: contextMenu.rename()
    }

    MenuSeparator {}

    // === Organization ===
    MenuItem {
        text: qsTr("Duplicate")
        icon.name: "edit-copy"
        onTriggered: contextMenu.duplicate()
    }

    MenuItem {
        text: qsTr("Open Folder")
        icon.name: "folder-open"
        onTriggered: contextMenu.openFolder()
    }

    MenuSeparator {}

    // === Export/Backup ===
    MenuItem {
        text: qsTr("Backup")
        icon.name: "document-save"
        onTriggered: contextMenu.backup()
    }

    MenuItem {
        text: qsTr("Export")
        icon.name: "document-save-as"
        onTriggered: contextMenu.exportInstance()
    }

    MenuSeparator {}

    // === Delete ===
    MenuItem {
        text: qsTr("Delete")
        icon.name: "edit-delete"
        palette.buttonText: ThemeColors.error  // Red for delete
        onTriggered: contextMenu.deleteInstance()
    }
}
