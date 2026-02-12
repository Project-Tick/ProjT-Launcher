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
#include "Common.h"

// Origin: Qt
// More specifically, this is a trimmed down version on the algorithm in:
// https://code.woboq.org/qt5/qtbase/src/widgets/styles/qcommonstyle.cpp.html#846
QList<std::pair<qreal, QString>> viewItemTextLayout(QTextLayout& textLayout, int lineWidth, qreal& height)
{
	QList<std::pair<qreal, QString>> lines;
	height = 0;

	textLayout.beginLayout();

	QString str = textLayout.text();
	while (true)
	{
		QTextLine line = textLayout.createLine();

		if (!line.isValid())
			break;
		if (line.textLength() == 0)
			break;

		line.setLineWidth(lineWidth);
		line.setPosition(QPointF(0, height));

		height += line.height();

		lines.append(std::make_pair(line.naturalTextWidth(), str.mid(line.textStart(), line.textLength())));
	}

	textLayout.endLayout();

	return lines;
}
