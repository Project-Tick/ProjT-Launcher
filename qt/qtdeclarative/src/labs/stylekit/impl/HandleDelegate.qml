// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

import QtQuick
import QtQuick.Templates as T
import Qt.labs.StyleKit
import Qt.labs.StyleKit.impl

DelegateContainer {
    /* The margins are not added to the handle itself, but to
     * the area (groove) where the handle moves. This is taken
     * care of by the control that uses the handle */
    leftMargin: 0
    rightMargin: 0
    topMargin: 0
    bottomMargin: 0

    delegateProperties: handleProperties

    required property StyleKitDelegateProperties handleProperties
}
