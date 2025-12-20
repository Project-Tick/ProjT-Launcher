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

Window {
    id: root
    visible: false
    modality: modal ? Qt.WindowModal : Qt.NonModal
    flags: Qt.Dialog | Qt.WindowTitleHint | Qt.WindowCloseButtonHint

    // Keep compatibility with Dialog API
    property bool modal: false
    property int standardButtons: Dialog.NoButton
    property int closePolicy: Popup.CloseOnEscape
    signal accepted()
    signal rejected()
    signal opened()

    default property alias data: body.data

    function open() { visible = true }
    function close() { visible = false }

    Keys.onEscapePressed: {
        if (closePolicy & Popup.CloseOnEscape) {
            root.close()
        }
    }

    onVisibleChanged: {
        if (visible) {
            opened()
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        Item {
            id: body
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        DialogButtonBox {
            id: buttonBox
            visible: standardButtons !== Dialog.NoButton
            standardButtons: root.standardButtons
            Layout.fillWidth: true
            onAccepted: {
                root.accepted()
                root.close()
            }
            onRejected: {
                root.rejected()
                root.close()
            }
        }
    }
}
