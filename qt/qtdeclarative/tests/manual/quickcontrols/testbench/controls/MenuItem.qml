// Copyright (C) 2026 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

QtObject {
    property string customControlName: qsTr("MenuItem")

    property var supportedStates: [
        [],
        ["mirrored"],
        ["display-icon-only"],
        ["display-icon-only", "mirrored"],
        ["display-text-only"],
        ["display-text-only", "mirrored"],
        ["display-text-beside-icon"],
        ["display-text-beside-icon", "mirrored"]
    ]

    property Component component: MenuItem {
        text: "MenuItem"
        icon.source: Utils.iconUrl
        display: {
            if (is("display-icon-only"))
                return MenuItem.IconOnly
            else if (is("display-text-only"))
                return MenuItem.TextOnly

            return MenuItem.TextBesideIcon
        }

        LayoutMirroring.enabled: is("mirrored")

        action: Action {
            shortcut: "Ctrl+A"
        }
    }
}
