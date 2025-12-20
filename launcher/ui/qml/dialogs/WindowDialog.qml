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
    property Item background: null
    property Item header: null
    property Item footer: null
    readonly property real availableWidth: width
    readonly property real availableHeight: height
    signal accepted()
    signal rejected()
    signal opened()
    signal closed()

    default property alias data: body.data

    property bool _wasVisible: false
    property real _headerHeight: 0
    property real _footerHeight: 0

    function open() { visible = true }
    function close() { visible = false }
    function accept() {
        accepted()
        close()
    }
    function reject() {
        rejected()
        close()
    }
    function _attachItem(item, host, fill) {
        if (!item || !host)
            return;
        item.parent = host;
        if (fill) {
            item.anchors.fill = host;
        }
    }

    onBackgroundChanged: _attachItem(background, backgroundHost, true)
    onHeaderChanged: {
        if (header) {
            _headerHeight = header.implicitHeight > 0 ? header.implicitHeight : header.height;
            header.parent = headerHost;
            header.anchors.left = headerHost.left;
            header.anchors.right = headerHost.right;
            header.anchors.top = headerHost.top;
        } else {
            _headerHeight = 0;
        }
    }
    onFooterChanged: {
        if (footer) {
            _footerHeight = footer.implicitHeight > 0 ? footer.implicitHeight : footer.height;
            footer.parent = footerHost;
            footer.anchors.left = footerHost.left;
            footer.anchors.right = footerHost.right;
            footer.anchors.bottom = footerHost.bottom;
        } else {
            _footerHeight = 0;
        }
    }

    onVisibleChanged: {
        if (visible && !_wasVisible) {
            opened()
        } else if (!visible && _wasVisible) {
            closed()
        }
        _wasVisible = visible
    }

    Item {
        id: backgroundHost
        anchors.fill: parent
        z: -1
    }

    FocusScope {
        id: keyScope
        anchors.fill: parent
        Keys.onEscapePressed: {
            if (closePolicy & Popup.CloseOnEscape) {
                root.close()
            }
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 12
            spacing: 8

            Item {
                id: headerHost
                visible: header !== null
                Layout.fillWidth: true
                Layout.preferredHeight: _headerHeight
            }

            Item {
                id: body
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            Item {
                id: footerHost
                visible: footer !== null
                Layout.fillWidth: true
                Layout.preferredHeight: _footerHeight
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

    Connections {
        target: footer
        ignoreUnknownSignals: true
        function onAccepted() {
            root.accepted();
            root.close();
        }
        function onRejected() {
            root.rejected();
            root.close();
        }
    }
}
