// Copyright (C) 2026 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

#ifndef QQUICKMENUITEMICONLABEL_P_H
#define QQUICKMENUITEMICONLABEL_P_H

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

#include <QtQuickControls2Impl/private/qquickiconlabel_p.h>

QT_BEGIN_NAMESPACE

class QQuickMenuItem;
class QQuickMenuItemIconLabelPrivate;

class Q_AUTOTEST_EXPORT QQuickMenuItemIconLabel : public QQuickIconLabel
{
    Q_OBJECT
    Q_PROPERTY(QQuickMenuItem *menuItem READ menuItem WRITE setMenuItem NOTIFY menuItemChanged FINAL)
    QML_NAMED_ELEMENT(MenuItemIconLabel)
    QML_ADDED_IN_VERSION(6, 12)

public:
    explicit QQuickMenuItemIconLabel(QQuickItem *parent = nullptr);
    ~QQuickMenuItemIconLabel();

    QQuickMenuItem *menuItem() const;
    void setMenuItem(QQuickMenuItem *menuItem);

signals:
    void menuItemChanged();

protected:
    void componentComplete() override;

private:
    Q_DISABLE_COPY(QQuickMenuItemIconLabel)
    Q_DECLARE_PRIVATE(QQuickMenuItemIconLabel)
};

QT_END_NAMESPACE

#endif // QQUICKMENUITEMICONLABEL_P_H
