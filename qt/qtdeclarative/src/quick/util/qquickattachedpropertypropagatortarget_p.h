// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QQUICKATTACHEDPROPERTYPROPAGATORTARGET_P_H
#define QQUICKATTACHEDPROPERTYPROPAGATORTARGET_P_H

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

#include <QtQuick/qtquickexports.h>

QT_BEGIN_NAMESPACE

class QQuickItem;

namespace QtPrivate {
class QQuickAttachedPropertyPropagator;
}

// Allows non-QQuickItem types like QQuickPopup to be used with
// QtPrivate::QQuickAttachedPropertyPropagator.
// ### Qt 7: make public: QTBUG-138546. We might also want to replace
// QQuickItemPrivate::attachedPropertyPropagator_parent with this,
// but then QQuickItem needs to derive from this.
class Q_QUICK_EXPORT QQuickAttachedPropertyPropagatorTarget
{
public:
    virtual ~QQuickAttachedPropertyPropagatorTarget() = default;

    virtual QQuickItem *attacheeItem() const = 0;
    virtual QtPrivate::QQuickAttachedPropertyPropagator *attachedParent(
        const QMetaObject *attachedType) const = 0;
};

// This is needed to be able to qobject_cast.
#define QQuickAttachedPropertyPropagatorTarget_iid "org.qt-project.Qt.QQuickAttachedPropertyPropagatorTarget"
Q_DECLARE_INTERFACE(QQuickAttachedPropertyPropagatorTarget, QQuickAttachedPropertyPropagatorTarget_iid)

QT_END_NAMESPACE

#endif // QQUICKATTACHEDPROPERTYPROPAGATORTARGET_P_H
