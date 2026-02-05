// Copyright (C) 2026 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick
import QtQuick.Shapes

ControlledShape {
    delegate: [
        PathPolyline {
            path: [ps.pt, Qt.point(80, 100), Qt.point(80, 0), Qt.point(60, 0),
                    Qt.point(60, 100), Qt.point(40, 100), Qt.point(40, 0), Qt.point(20, 100), pe.pt]
        }
    ]
    ControlPoint {
        id: ps
        cx: 100
        cy: 100
    }
    ControlPoint {
        id: pe
        cx: 0
        cy: 100
    }
}
