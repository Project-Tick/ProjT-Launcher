// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

#include "qquickmacfocusframe_p.h"

#include <private/qcoregraphics_p.h>

#include <QtGui/qguiapplication.h>

#include <QtQml/qqmlcomponent.h>

#include <AppKit/AppKit.h>

QT_BEGIN_NAMESPACE

QQuickItem *QQuickMacFocusFrame::createFocusFrame(QQmlContext *context)
{
    QQmlComponent component(
            context->engine(),
            QUrl(QStringLiteral(
                    "qrc:/qt-project.org/imports/QtQuick/NativeStyle/util/MacFocusFrame.qml")));
    auto frame = qobject_cast<QQuickItem *>(component.create());
    if (!frame)
        return nullptr;

    auto indicatorColor = qt_mac_toQColor(NSColor.keyboardFocusIndicatorColor.CGColor);
    indicatorColor.setAlpha(255);
    frame->setProperty("systemFrameColor", indicatorColor);
    return frame;
}

QT_END_NAMESPACE
