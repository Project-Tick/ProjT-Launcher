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
import "../Theme.js" as Theme

/**
 * InstanceContextMenu – Right-click Menu for Instance Actions
 * 
 * Provides all instance-related actions:
 * - Launch
 * - Create New Instance
 * - Import Instance
 * - Edit Settings
 * - Rename
 * - Duplicate
 * - Open Folder
 * - Backup
 * - Export
 * - Delete
 * 
 * Usage:
 *   InstanceContextMenu {
 *       id: contextMenu
 *       instanceId: selectedInstanceId
 *       onEditSettings: { ... }
 *       onDelete: { ... }
 *   }
 *   
 *   onRightClicked: contextMenu.popup(mouse.x, mouse.y)
 */

Menu {
    id: contextMenu
    
    // Properties
    property string instanceId: ""
    property bool canLaunch: true
    property bool isRunning: false
    
    // Signals
    signal launch()
    signal editSettings()
    signal rename()
    signal duplicate()
    signal openFolder()
    signal backup()
    signal exportInstance()
    signal deleteInstance()
    signal createNew()
    signal importInstance()
    
    // Styling
    background: Rectangle {
        color: Theme.surface
        border.color: "#323742"
        border.width: 1
        radius: Theme.radius
    }
    
    contentItem: ListView {
        implicitWidth: 200
        implicitHeight: contentHeight
        model: contextMenu.contentModel
        ScrollIndicator.vertical: ScrollIndicator { }
    }
    
    // === Launch ===
    MenuItem {
        text: contextMenu.isRunning ? qsTr("Kill") : qsTr("Launch")
        icon.name: contextMenu.isRunning ? "process-stop" : "media-playback-start"
        enabled: contextMenu.canLaunch
        onTriggered: contextMenu.launch()
    }
    
    MenuSeparator { }
    
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
    
    MenuSeparator { }
    
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
    
    MenuSeparator { }
    
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
    
    MenuSeparator { }
    
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
    
    MenuSeparator { }
    
    // === Delete ===
    MenuItem {
        text: qsTr("Delete")
        icon.name: "edit-delete"
        palette.buttonText: "#ff6b6b"  // Red for delete
        onTriggered: contextMenu.deleteInstance()
    }
}
