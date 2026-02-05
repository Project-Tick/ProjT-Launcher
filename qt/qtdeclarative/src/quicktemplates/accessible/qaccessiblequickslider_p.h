// Copyright (C) 2026 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

#ifndef QACCESSIBLEQUICKSLIDER_H
#define QACCESSIBLEQUICKSLIDER_H

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

#include "qaccessiblequickcontrol_p.h"

QT_BEGIN_NAMESPACE

class QQuickSlider;

class QAccessibleQuickSlider : public QAccessibleQuickControl
{
public:
    QAccessibleQuickSlider(QQuickSlider *slider);

    QList<QAccessible::Attribute> attributeKeys() const override;
    QVariant attributeValue(QAccessible::Attribute key) const override;

private:
    QQuickSlider *slider() const;
};

QT_END_NAMESPACE

#endif // QACCESSIBLEQUICKSLIDER_H
