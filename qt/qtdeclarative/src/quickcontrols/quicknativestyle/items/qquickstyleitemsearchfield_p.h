// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

#ifndef QQUICKSTYLEITEMSEARCHFIELD_P_H
#define QQUICKSTYLEITEMSEARCHFIELD_P_H

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
#include <private/qquicksearchfield_p.h>

QT_BEGIN_NAMESPACE

class QQuickStyleItemSearchField : public QQuickStyleItem
{
    Q_OBJECT

    Q_PROPERTY(SubControl subControl MEMBER m_subControl)

    QML_NAMED_ELEMENT(SearchField)

public:
    enum SubControl {
        Frame = 1,
        Search,
        Clear
    };
    Q_ENUM(SubControl)

    QFont styleFont(QQuickItem *control) const override;

protected:
    void connectToControl() const override;
    void paintEvent(QPainter *painter) const override;
    StyleItemGeometry calculateGeometry() override;

private:
    void initStyleOption(QStyleOptionSearchField &styleOption) const;

private:
    SubControl m_subControl = Frame;
};

QT_END_NAMESPACE

#endif // QQUICKSTYLEITEMSEARCHFIELD_P_H
