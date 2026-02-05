// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qqstylekitcustomtheme_p.h"
#include "qqstylekittheme_p.h"

QT_BEGIN_NAMESPACE

QQStyleKitCustomTheme::QQStyleKitCustomTheme(QObject *parent)
    : QObject(parent)
{
}

QString QQStyleKitCustomTheme::name() const
{
    return m_name;
}

void QQStyleKitCustomTheme::setName(const QString &newName)
{
    if (m_name == newName)
        return;
    m_name = newName;
    emit nameChanged();
}

QQmlComponent *QQStyleKitCustomTheme::theme() const
{
    return m_theme;
}

void QQStyleKitCustomTheme::setTheme(QQmlComponent *newTheme)
{
    if (m_theme == newTheme)
        return;
    m_theme = newTheme;
    emit themeChanged();
}

QT_END_NAMESPACE

#include "moc_qqstylekitcustomtheme_p.cpp"
