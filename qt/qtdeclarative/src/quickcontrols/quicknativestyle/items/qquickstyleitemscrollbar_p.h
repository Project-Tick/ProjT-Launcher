// Copyright (C) 2020 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

#ifndef QQUICKSTYLEITEMSCROLLBAR_P_H
#define QQUICKSTYLEITEMSCROLLBAR_P_H

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

#include <private/qquickscrollbar_p.h>
#include <private/qquickstyleitem_p.h>

QT_BEGIN_NAMESPACE

class QQuickStyleItemScrollBar : public QQuickStyleItem
{
    Q_OBJECT

    Q_PROPERTY(SubControl subControl MEMBER m_subControl)

    QML_NAMED_ELEMENT(ScrollBar)

public:
    enum SubControl {
        Groove = 1,
        Handle,
        AddLine,
        SubLine
    };
    Q_ENUM(SubControl)

    explicit QQuickStyleItemScrollBar(QQuickItem *parent = nullptr);

    QFont styleFont(QQuickItem *control) const override;

protected:
    void connectToControl() const override;
    void paintEvent(QPainter *painter) const override;
    StyleItemGeometry calculateGeometry() override;

private:
    void initStyleOption(QStyleOptionSlider &styleOption) const;

private:
   SubControl m_subControl = Groove;
};

QT_END_NAMESPACE

#endif // QQUICKSTYLEITEMSCROLLBAR_P_H
