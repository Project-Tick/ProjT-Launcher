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

#include <QStyledItemDelegate>

/* Custom data types for our custom list models :) */
enum UserDataTypes {
    TITLE = 257,        // QString
    DESCRIPTION = 258,  // QString
    INSTALLED = 259     // bool
};

/** This is an item delegate composed of:
 *  - An Icon on the left
 *  - A title
 *  - A description
 * */
class ProjectItemDelegate final : public QStyledItemDelegate {
    Q_OBJECT

   public:
    ProjectItemDelegate(QWidget* parent);

    void paint(QPainter*, const QStyleOptionViewItem&, const QModelIndex&) const override;

    bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) override;

   signals:
    void checkboxClicked(const QModelIndex& index);

   private:
    QStyleOptionViewItem makeCheckboxStyleOption(const QStyleOptionViewItem& opt, const QStyle* style) const;
};
