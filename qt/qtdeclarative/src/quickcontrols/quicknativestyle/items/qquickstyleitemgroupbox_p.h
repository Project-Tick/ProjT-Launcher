// Copyright (C) 2020 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

#ifndef QQUICKSTYLEITEMGROUPBOX_P_H
#define QQUICKSTYLEITEMGROUPBOX_P_H

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

#include <private/qquickgroupbox_p.h>
#include <private/qquickstyleitem_p.h>

QT_BEGIN_NAMESPACE

class QQuickStyleItemGroupBox : public QQuickStyleItem
{
    Q_OBJECT
    Q_PROPERTY(QQuickStyleMargins groupBoxPadding READ groupBoxPadding NOTIFY groupBoxPaddingChanged)
    Q_PROPERTY(QPointF labelPos READ labelPos NOTIFY labelPosChanged)
    QML_NAMED_ELEMENT(GroupBox)

public:
    QQuickStyleMargins groupBoxPadding() const;
    QPointF labelPos() const;
    QFont styleFont(QQuickItem *control) const override;

Q_SIGNALS:
    void groupBoxPaddingChanged();
    void labelPosChanged();

protected:
    void paintEvent(QPainter *painter) const override;
    StyleItemGeometry calculateGeometry() override;

private:
    QQuickStyleMargins m_groupBoxPadding;
    QPointF m_labelPos;

    void initStyleOption(QStyleOptionGroupBox &styleOption) const;
};

QT_END_NAMESPACE

#endif // QQUICKSTYLEITEMGROUPBOX_P_H
