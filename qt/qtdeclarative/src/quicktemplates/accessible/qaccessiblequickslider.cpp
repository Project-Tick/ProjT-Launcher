// Copyright (C) 2026 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

#include "qaccessiblequickslider_p.h"
#include "qquickslider_p.h"

QT_BEGIN_NAMESPACE

QAccessibleQuickSlider::QAccessibleQuickSlider(QQuickSlider *slider)
    : QAccessibleQuickControl(slider)
{
}

QList<QAccessible::Attribute> QAccessibleQuickSlider::attributeKeys() const
{
    QList<QAccessible::Attribute> keys = QAccessibleQuickControl::attributeKeys();
    keys.append(QAccessible::Attribute::Orientation);

    return keys;
}

QVariant QAccessibleQuickSlider::attributeValue(QAccessible::Attribute key) const
{
    if (key == QAccessible::Attribute::Orientation)
        return QVariant::fromValue(slider()->orientation());

    return QAccessibleQuickControl::attributeValue(key);
}

QQuickSlider *QAccessibleQuickSlider::slider() const
{
    return static_cast<QQuickSlider *>(object());
}

QT_END_NAMESPACE
