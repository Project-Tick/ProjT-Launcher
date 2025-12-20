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
import QtQuick.Layouts 1.15
import ProjTLauncher 1.0
import "../Theme.js" as Theme

WindowDialog {
    id: copyInstanceDialog
    title: qsTr("Copy Instance")
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel | Dialog.Help
    width: 575
    height: 695

    property var sourceInstance: null
    property string newName: ""
    property string newGroup: ""
    property bool keepPlaytime: true
    property bool copyWorlds: true
    property bool copyResourcePacks: true
    property bool copyShaderPacks: true
    property bool copyMods: true
    property bool copyScreenshots: false
    property bool copyServers: true
    property bool copyGameOptions: true
    // Advanced options
    property bool useSymbolicLinks: false
    property bool useHardLinks: false
    property bool recursiveLinks: false
    property bool dontLinkSaves: false
    property bool useClone: false
    property bool cloneSupported: false

    ScrollView {
        anchors.fill: parent
        clip: true

        ColumnLayout {
            width: copyInstanceDialog.availableWidth - 20
            spacing: Theme.spacingS

            // Icon button centered
            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: 100

                ToolButton {
                    id: iconButton
                    anchors.centerIn: parent
                    width: 80
                    height: 80

                    Image {
                        anchors.fill: parent
                        anchors.margins: 4
                        source: sourceInstance ? sourceInstance.iconPath : "qrc:/icons/instances/grass"
                        fillMode: Image.PreserveAspectFit
                    }

                    onClicked: {
                        // Open icon picker
                        if (ProjT && ProjT.showIconPicker) {
                            ProjT.showIconPicker();
                        }
                    }
                }
            }

            // Instance name
            TextField {
                id: nameField
                Layout.fillWidth: true
                placeholderText: qsTr("Name")
                text: newName || (sourceInstance ? sourceInstance.name + " (Copy)" : "")
                onTextChanged: newName = text
            }

            Rectangle {
                Layout.fillWidth: true
                height: 1
                color: ThemeColors.border
            }

            // Group selection
            RowLayout {
                Layout.fillWidth: true

                Label {
                    text: qsTr("&Group")
                }

                ComboBox {
                    id: groupCombo
                    Layout.fillWidth: true
                    editable: true
                    model: ProjT && ProjT.instancesVM ? ProjT.instancesVM.groupList : []
                    editText: newGroup
                    onEditTextChanged: newGroup = editText
                }
            }

            // Instance Copy Options
            GroupBox {
                Layout.fillWidth: true
                title: qsTr("Instance Copy Options")

                GridLayout {
                    anchors.fill: parent
                    columns: 2
                    columnSpacing: Theme.spacingM
                    rowSpacing: Theme.spacingS

                    CheckBox {
                        text: qsTr("Keep play time")
                        checked: keepPlaytime
                        onCheckedChanged: keepPlaytime = checked
                    }

                    CheckBox {
                        text: qsTr("Copy screenshots")
                        checked: copyScreenshots
                        onCheckedChanged: copyScreenshots = checked
                    }

                    CheckBox {
                        text: qsTr("Copy saves")
                        checked: copyWorlds
                        onCheckedChanged: copyWorlds = checked
                    }

                    CheckBox {
                        text: qsTr("Copy shader packs")
                        checked: copyShaderPacks
                        onCheckedChanged: copyShaderPacks = checked
                    }

                    CheckBox {
                        text: qsTr("Copy game options")
                        checked: copyGameOptions
                        onCheckedChanged: copyGameOptions = checked
                        ToolTip.visible: hovered
                        ToolTip.text: qsTr("Copy the in-game options like FOV, max framerate, etc.")
                    }

                    CheckBox {
                        text: qsTr("Copy servers")
                        checked: copyServers
                        onCheckedChanged: copyServers = checked
                    }

                    CheckBox {
                        text: qsTr("Copy resource packs")
                        checked: copyResourcePacks
                        onCheckedChanged: copyResourcePacks = checked
                    }

                    CheckBox {
                        text: qsTr("Copy mods")
                        checked: copyMods
                        onCheckedChanged: copyMods = checked
                        ToolTip.visible: hovered
                        ToolTip.text: qsTr("Disabling this will still keep the mod loader (ex: Fabric, Quilt, etc.) but erase the mods folder and their configs.")
                    }

                    Item {
                        width: 1
                        height: 1
                    }

                    CheckBox {
                        id: selectAllCheckbox
                        text: qsTr("Select all")
                        checked: false
                        onCheckedChanged: {
                            if (checked) {
                                keepPlaytime = true;
                                copyScreenshots = true;
                                copyWorlds = true;
                                copyShaderPacks = true;
                                copyGameOptions = true;
                                copyServers = true;
                                copyResourcePacks = true;
                                copyMods = true;
                            }
                        }
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                height: 1
                color: ThemeColors.border
            }

            // Advanced Copy Options header
            Label {
                Layout.fillWidth: true
                text: qsTr("Advanced Copy Options")
                horizontalAlignment: Text.AlignHCenter
                font.bold: true
            }

            // Symbolic and Hard Link Options
            GroupBox {
                Layout.fillWidth: true
                title: qsTr("Symbolic and Hard Link Options")

                ColumnLayout {
                    anchors.fill: parent
                    spacing: Theme.spacingS

                    Label {
                        Layout.fillWidth: true
                        text: qsTr("Links are supported on most filesystems except FAT")
                        horizontalAlignment: Text.AlignHCenter
                        color: ThemeColors.textSecondary
                        font.pointSize: Theme.fontSizeSmall
                    }

                    GridLayout {
                        Layout.fillWidth: true
                        columns: 2
                        columnSpacing: Theme.spacingM
                        rowSpacing: Theme.spacingS

                        CheckBox {
                            id: symbolicLinksCheck
                            text: qsTr("Use symbolic links")
                            checked: useSymbolicLinks
                            onCheckedChanged: {
                                useSymbolicLinks = checked;
                                if (checked)
                                    useHardLinks = false;
                            }
                            ToolTip.visible: hovered
                            ToolTip.text: qsTr("Use symbolic links instead of copying files.")
                        }

                        CheckBox {
                            id: recursiveLinkCheck
                            text: qsTr("Link files recursively")
                            enabled: useSymbolicLinks || useHardLinks
                            checked: recursiveLinks
                            onCheckedChanged: recursiveLinks = checked
                            ToolTip.visible: hovered
                            ToolTip.text: qsTr("Link each resource individually instead of linking whole folders at once")
                        }

                        CheckBox {
                            id: hardLinksCheck
                            text: qsTr("Use hard links")
                            checked: useHardLinks
                            onCheckedChanged: {
                                useHardLinks = checked;
                                if (checked)
                                    useSymbolicLinks = false;
                            }
                            ToolTip.visible: hovered
                            ToolTip.text: qsTr("Use hard links instead of copying files.")
                        }

                        CheckBox {
                            id: dontLinkSavesCheck
                            text: qsTr("Don't link saves")
                            enabled: useSymbolicLinks || useHardLinks
                            checked: dontLinkSaves
                            onCheckedChanged: dontLinkSaves = checked
                            ToolTip.visible: hovered
                            ToolTip.text: qsTr("If \"copy saves\" is selected world save data will be copied instead of linked and thus not shared between instances.")
                        }
                    }
                }
            }

            // CoW Options
            GroupBox {
                Layout.fillWidth: true
                title: qsTr("CoW (Copy-on-Write) Options")

                RowLayout {
                    anchors.fill: parent
                    spacing: Theme.spacingM

                    CheckBox {
                        id: cloneCheck
                        text: qsTr("Clone instead of copying")
                        enabled: cloneSupported
                        checked: useClone
                        onCheckedChanged: useClone = checked
                        ToolTip.visible: hovered
                        ToolTip.text: qsTr("Files cloned with reflinks take up no extra space until they are modified.")
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    Label {
                        text: cloneSupported ? qsTr("Reflinks supported") : qsTr("Your filesystem and/or OS doesn't support reflinks")
                        color: cloneSupported ? ThemeColors.success : ThemeColors.textSecondary
                        font.pointSize: Theme.fontSizeSmall
                    }
                }
            }
        }
    }

    onAccepted: {
        if (sourceInstance && ProjT && ProjT.instancesVM) {
            ProjT.instancesVM.copyInstance(sourceInstance.id, nameField.text, groupCombo.editText, {
                keepPlaytime: keepPlaytime,
                worlds: copyWorlds,
                mods: copyMods,
                resourcePacks: copyResourcePacks,
                shaderPacks: copyShaderPacks,
                servers: copyServers,
                screenshots: copyScreenshots,
                gameOptions: copyGameOptions,
                useSymbolicLinks: useSymbolicLinks,
                useHardLinks: useHardLinks,
                recursiveLinks: recursiveLinks,
                dontLinkSaves: dontLinkSaves,
                useClone: useClone
            });
        }
    }

    Component.onCompleted: {
        // Check if CoW/reflinks are supported
        if (ProjT && ProjT.fileSystem && ProjT.fileSystem.supportsReflinks) {
            cloneSupported = ProjT.fileSystem.supportsReflinks();
        }
    }
}
