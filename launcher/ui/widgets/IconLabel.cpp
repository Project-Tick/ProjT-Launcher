// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  This file is part of ProjT Launcher and is licensed under
 *  the GNU General Public License version 3 or later.
 *
 *  If this file includes work from previous open-source projects,
 *  their original copyright and license notices are preserved below.
 */
#include "IconLabel.h"

#include <QLayout>
#include <QPainter>
#include <QRect>
#include <QStyle>
#include <QStyleOption>

IconLabel::IconLabel(QWidget* parent, QIcon icon, QSize size) : QWidget(parent), m_size(size), m_icon(icon)
{
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

QSize IconLabel::sizeHint() const
{
    return m_size;
}

void IconLabel::setIcon(QIcon icon)
{
    m_icon = icon;
    update();
}

void IconLabel::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    QRect rect = contentsRect();
    int width = rect.width();
    int height = rect.height();
    if (width < height) {
        rect.setHeight(width);
        rect.translate(0, (height - width) / 2);
    } else if (width > height) {
        rect.setWidth(height);
        rect.translate((width - height) / 2, 0);
    }
    m_icon.paint(&p, rect);
}
