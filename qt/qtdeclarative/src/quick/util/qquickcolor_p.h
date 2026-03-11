// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

#ifndef QQUICKCOLOR_P_H
#define QQUICKCOLOR_P_H

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

#include <private/qtquickglobal_p.h>
#include <private/qv4engine_p.h>

#include <QtCore/qobject.h>
#include <QtGui/qcolor.h>
#include <QtQml/qqml.h>

QT_BEGIN_NAMESPACE

class Q_QUICK_EXPORT QQuickColor : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(Color)
    QML_SINGLETON
    QML_ADDED_IN_VERSION(6, 12)

public:
    explicit QQuickColor(QObject *parent = nullptr);

    Q_INVOKABLE QColor rgba(double r, double g, double b, double a) const;
    Q_INVOKABLE QColor hsla(double h, double s, double l, double a) const;
    Q_INVOKABLE QColor hsva(double h, double s, double v, double a) const;

    Q_INVOKABLE QColor fromString(const QString &name) const;
    Q_INVOKABLE bool equal(const QColor &lhs, const QColor &rhs) const;

    Q_INVOKABLE QColor transparent(const QColor &color, qreal opacity) const;
    Q_INVOKABLE QColor blend(const QColor &a, const QColor &b, qreal factor) const;
    Q_INVOKABLE QColor darker(const QColor &baseColor, double factor) const;
    Q_INVOKABLE QColor lighter(const QColor &baseColor, double factor) const;
    Q_INVOKABLE QColor tint(const QColor &baseColor, const QColor &tintColor) const;
};

QT_END_NAMESPACE

#endif // QQUICKCOLOR_P_H
