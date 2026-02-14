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
    Attached.foo: "W"

    Item {
        objectName: "i0"
        Attached.objectName: objectName + "Attached"
        Attached.foo: "W/0"
        Item {
            objectName: "i1"
            Item {
                objectName: "i3"
                Attached.objectName: objectName + "Attached"
                Item {
                    objectName: "i5"
                    Attached.objectName: objectName + "Attached"
                }
                Item {
                    objectName: "i6"
                    Attached.objectName: objectName + "Attached"
                    Attached.foo: "W/0/1/3/6"
                }
            }
        }
    }
}
