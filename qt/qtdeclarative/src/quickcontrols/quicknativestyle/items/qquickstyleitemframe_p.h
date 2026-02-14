// Copyright (C) 2020 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

#ifndef QQUICKSTYLEITEMFRAME_P_H
#define QQUICKSTYLEITEMFRAME_P_H

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

#include <private/qquickframe_p.h>
#include <private/qquickstyleitem_p.h>

QT_BEGIN_NAMESPACE

class QQuickStyleItemFrame : public QQuickStyleItem
{
    Q_OBJECT
    QML_NAMED_ELEMENT(Frame)

protected:
    void paintEvent(QPainter *painter) const override;
    StyleItemGeometry calculateGeometry() override;

private:
    void initStyleOption(QStyleOptionFrame &styleOption) const;
};

QT_END_NAMESPACE

#endif // QQUICKSTYLEITEMFRAME_P_H
