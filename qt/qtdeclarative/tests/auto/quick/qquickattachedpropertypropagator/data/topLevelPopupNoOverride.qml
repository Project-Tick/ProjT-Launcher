// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick
import QtQuick.Window
import QtQuick.Controls

import Test

ApplicationWindow {
    width: 400
    height: 400
    objectName: "window"

    Attached.objectName: objectName + "Attached"
    Attached.foo: "1"

    Popup {
        objectName: "topLevelPopupNoOverride"

        Popup {
            objectName: "noOverrideChildPopup"
        }

        Item {
            objectName: "noOverrideChildItem"
        }

        Popup {
            objectName: "overrideChildPopup"

            Attached.objectName: objectName + "Attached"
            Attached.foo: "2.3"
        }

        Item {
            objectName: "overrideChildItem"

            Attached.objectName: objectName + "Attached"
            Attached.foo: "2.4"
        }
    }
}
