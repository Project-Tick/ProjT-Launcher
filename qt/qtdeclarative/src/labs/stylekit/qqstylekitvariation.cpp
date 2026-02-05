// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qqstylekitvariation_p.h"

QT_BEGIN_NAMESPACE

int QQStyleKitVariation::s_variationCount = 0;

QQStyleKitVariation::QQStyleKitVariation(QObject *parent)
    : QQStyleKitControls(parent)
{
    /* As an optimization, keep track of how many variations that are defined. That way
     * we skip looking for them later if s_variationCount == 0. */
    ++s_variationCount;
}

QString QQStyleKitVariation::name() const
{
    return m_name;
}

void QQStyleKitVariation::setName(const QString &name)
{
    if (m_name == name)
        return;

    m_name = name;
    emit nameChanged();
}

QT_END_NAMESPACE

#include "moc_qqstylekitvariation_p.cpp"
