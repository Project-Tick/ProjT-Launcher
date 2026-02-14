// Copyright (C) 2020 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

#ifndef QQUICKSTYLEITEMTEXTFIELD_P_H
#define QQUICKSTYLEITEMTEXTFIELD_P_H

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
#include <private/qquicktextfield_p.h>

QT_BEGIN_NAMESPACE

class QQuickStyleItemTextField : public QQuickStyleItem
{
    Q_OBJECT
    QML_NAMED_ELEMENT(TextField)

public:
    QFont styleFont(QQuickItem *control) const override;

protected:
    void connectToControl() const override;
    void paintEvent(QPainter *painter) const override;
    StyleItemGeometry calculateGeometry() override;

private:
    void initStyleOption(QStyleOptionFrame &styleOption) const;
};

QT_END_NAMESPACE

#endif // QQUICKSTYLEITEMTEXTFIELD_P_H
