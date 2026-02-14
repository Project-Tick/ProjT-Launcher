// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QQUICKATTACHEDPROPERTYPROPAGATOR_P_H
#define QQUICKATTACHEDPROPERTYPROPAGATOR_P_H

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

#include <QtCore/qobject.h>
#include <QtCore/qloggingcategory.h>
#include <QtQuick/qtquickexports.h>

QT_BEGIN_NAMESPACE

QT_DECLARE_EXPORTED_QT_LOGGING_CATEGORY(lcAttachedPropertyPropagator, Q_QUICK_EXPORT)

// This type is a copy of the type of the same name from Controls.
// It exists here because QQuickContextMenu needs to derive from it,
// but that type is in Templates, which can't depend on Controls.

// This allows us to use the same name as the type in Controls.
namespace QtPrivate {

class QQuickAttachedPropertyPropagatorPrivate;

class Q_QUICK_EXPORT QQuickAttachedPropertyPropagator : public QObject
{
    Q_OBJECT

public:
    explicit QQuickAttachedPropertyPropagator(QObject *parent = nullptr);
    ~QQuickAttachedPropertyPropagator();

    QList<QQuickAttachedPropertyPropagator *> attachedChildren() const;

    QQuickAttachedPropertyPropagator *attachedParent() const;

    static QQuickAttachedPropertyPropagator *attachedObject(const QMetaObject *attachedType,
        QObject *attachee, bool create = false);
    static QQuickAttachedPropertyPropagator *fallbackAttachedObject(
        const QMetaObject *attachedType, QObject *attachee);

protected:
    QQuickAttachedPropertyPropagator(QQuickAttachedPropertyPropagatorPrivate &dd, QObject *parent);

    void initialize();

    virtual void attachedParentChange(QQuickAttachedPropertyPropagator *newParent,
        QQuickAttachedPropertyPropagator *oldParent);

private:
#ifndef QT_NO_DEBUG_STREAM
    friend Q_QUICK_EXPORT QDebug operator<<(QDebug debug,
        const QQuickAttachedPropertyPropagator *propagator);
#endif

    Q_DECLARE_PRIVATE(QQuickAttachedPropertyPropagator)
};

} // namespace QtPrivate

QT_END_NAMESPACE

#endif // QQUICKATTACHEDPROPERTYPROPAGATOR_P_H
