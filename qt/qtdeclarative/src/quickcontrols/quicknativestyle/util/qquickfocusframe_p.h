// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

#ifndef QQUICKFOCUSFRAME_P_H
#define QQUICKFOCUSFRAME_P_H

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

#include <private/qquickstyleitem_p.h>

#include <QtQuickNativeStyle/qtquicknativestyleexports.h>

#include <QtQuick/qquickitem.h>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(lcFocusFrame)

struct QQuickFocusFrameDescription {
    QQuickItem *target;
    QQuickStyleMargins margins;
    const qreal radius = 3;
    bool isValid() const { return target != nullptr; }
    static QQuickFocusFrameDescription Invalid;
};

class Q_QUICKNATIVESTYLE_EXPORT QQuickFocusFrame : public QObject
{
    Q_OBJECT

public:
    QQuickFocusFrame();

private:
    static QScopedPointer<QQuickItem> m_focusFrame;

    virtual QQuickItem *createFocusFrame(QQmlContext *context) = 0;
    void moveToItem(QQuickItem *item);
    QQuickFocusFrameDescription getDescriptionForItem(QQuickItem *focusItem) const;
};

QT_END_NAMESPACE

#endif // QQUICKFOCUSFRAME_P_H
