// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

#ifndef QQUICKWINDOWSFOCUSFRAME_P_H
#define QQUICKWINDOWSFOCUSFRAME_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <private/qquickfocusframe_p.h>

#include <QtQuickNativeStyle/qtquicknativestyleexports.h>

QT_BEGIN_NAMESPACE

class Q_QUICKNATIVESTYLE_EXPORT QQuickWindowsFocusFrame : public QQuickFocusFrame
{
    Q_OBJECT

public:
    QQuickWindowsFocusFrame() = default;

private:
    virtual QQuickItem *createFocusFrame(QQmlContext *context) override;
};

QT_END_NAMESPACE

#endif // QQUICKWINDOWSFOCUSFRAME_P_H
