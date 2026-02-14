// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QQUICKTOOLTIP_P_P_H
#define QQUICKTOOLTIP_P_P_H

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

#include <QtQuick/private/qquickattachedpropertypropagator_p_p.h>
#include <QtQuickTemplates2/private/qquicktooltip_p.h>
#include <QtQml/private/qqmlpropertyutils_p.h>

QT_BEGIN_NAMESPACE

class Q_AUTOTEST_EXPORT QQuickToolTipAttachedPrivate
    : public QtPrivate::QQuickAttachedPropertyPropagatorPrivate
{
    Q_DECLARE_PUBLIC(QQuickToolTipAttached)

public:
    QQuickToolTip *instance(bool create) const;

    static QQuickToolTipAttachedPrivate *get(QQuickToolTipAttached *toolTipAttached)
    {
        return toolTipAttached->d_func();
    }

    static void maybeSetVisibleImplicitly(const QObject *attachee, bool visible);
    void setVisible(bool visible, QQml::PropertyUtils::State propertyState);
    bool isVisibleExplicitlySet() const;

    void setDelay(int delay, QQml::PropertyUtils::State propertyState);
    bool isDelayExplicitlySet() const;

    void setTimeout(int timeout, QQml::PropertyUtils::State propertyState);
    bool isTimeoutExplicitlySet() const;

    void inheritPolicy(QQuickToolTip::Policy policy);
    void propagatePolicy();

    bool warnIfAttacheeIsNotAnItem(const QString &functionName);

    static int calculateTimeout(const QString &text);

    bool complete = true;
    bool explicitVisible = false;
    bool pendingShow = false;
    bool explicitDelay = false;
    bool explicitTimeout = false;
    int delay = 0;
    int timeout = -1;
    QString text;
    QQuickToolTip::Policy policy = QQuickToolTip::Automatic;
};

QT_END_NAMESPACE

#endif // QQUICKTOOLTIP_P_P_H
