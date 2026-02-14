// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QQUICKATTACHEDPROPERTYPROPAGATOR_P_P_H
#define QQUICKATTACHEDPROPERTYPROPAGATOR_P_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qquickattachedpropertypropagator_p.h"

#include <QtCore/qpointer.h>
#include <QtCore/private/qobject_p.h>
#include <QtQuick/private/qquickitemchangelistener_p.h>

QT_BEGIN_NAMESPACE

class QWindow;
class QQuickWindow;

namespace QtPrivate {

class Q_QUICK_EXPORT QQuickAttachedPropertyPropagatorPrivate
    : public QObjectPrivate
    , public QSafeQuickItemChangeListener<QQuickAttachedPropertyPropagatorPrivate>
{
public:
    Q_DECLARE_PUBLIC(QQuickAttachedPropertyPropagator)

    static QQuickAttachedPropertyPropagatorPrivate *get(QQuickAttachedPropertyPropagator *attachedObject)
    {
        return attachedObject->d_func();
    }

    void init();

    void attachTo(QObject *object);
    void detachFrom(QObject *object);
    void setAttachedParent(QQuickAttachedPropertyPropagator *parent);

    void itemWindowChanged(QQuickWindow *window);
    void transientParentWindowChanged(QWindow *newTransientParent);
    void itemParentChanged(QQuickItem *item, QQuickItem *parent) override;

    static const QMetaObject *firstCppMetaObject(QQuickAttachedPropertyPropagator *propagator);
    static QQuickAttachedPropertyPropagator *findAttachedParent(const QMetaObject *attachedType,
        QObject *attachee);
    static QList<QQuickAttachedPropertyPropagator *> findAttachedChildren(
        const QMetaObject *attachedType, QObject *object);
    static QQuickItem *findAttachedItem(QObject *object);

    QList<QQuickAttachedPropertyPropagator *> attachedChildren;
    QPointer<QQuickAttachedPropertyPropagator> attachedParent;
    const QMetaObject *attacherMetaObject = nullptr;
};

} // namespace QtPrivate

QT_END_NAMESPACE

#endif // QQUICKATTACHEDPROPERTYPROPAGATOR_P_P_H
