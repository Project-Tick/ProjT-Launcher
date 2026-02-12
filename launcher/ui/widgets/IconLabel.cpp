// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2026 Project Tick
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
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
	int width  = rect.width();
	int height = rect.height();
	if (width < height)
	{
		rect.setHeight(width);
		rect.translate(0, (height - width) / 2);
	}
	else if (width > height)
	{
		rect.setWidth(height);
		rect.translate((width - height) / 2, 0);
	}
	m_icon.paint(&p, rect);
}
