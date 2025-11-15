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
#pragma once
#include <QIcon>
#include <QWidget>

class QStyleOption;

/**
 * This is a trivial widget that paints a QIcon of the specified size.
 */
class IconLabel : public QWidget {
    Q_OBJECT

   public:
    /// Create a line separator. orientation is the orientation of the line.
    explicit IconLabel(QWidget* parent, QIcon icon, QSize size);

    virtual QSize sizeHint() const;
    virtual void paintEvent(QPaintEvent*);

    void setIcon(QIcon icon);

   private:
    QSize m_size;
    QIcon m_icon;
};
