// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

pragma Singleton

import QtQuick
import Qt.labs.StyleKit
import Qt.labs.StyleKit.impl

QtObject {
    readonly property Component defaultDelegate: StyleKitDelegate {}
    readonly property Component defaultShadowDelegate: Shadow {}
}
