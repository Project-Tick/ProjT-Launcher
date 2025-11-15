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
#include <QAbstractItemView>
#include <QPlainTextEdit>

class QAbstractItemModel;

class LogView : public QPlainTextEdit {
    Q_OBJECT
   public:
    explicit LogView(QWidget* parent = nullptr);
    virtual ~LogView();

    virtual void setModel(QAbstractItemModel* model);
    QAbstractItemModel* model() const;

   public slots:
    void setWordWrap(bool wrapping);
    void setColorLines(bool colorLines);
    void findNext(const QString& what, bool reverse);
    void scrollToBottom();

   protected slots:
    void repopulate();
    // note: this supports only appending
    void rowsInserted(const QModelIndex& parent, int first, int last);
    void rowsAboutToBeInserted(const QModelIndex& parent, int first, int last);
    // note: this supports only removing from front
    void rowsRemoved(const QModelIndex& parent, int first, int last);
    void modelDestroyed(QObject* model);

   protected:
    QAbstractItemModel* m_model = nullptr;
    QTextCharFormat* m_defaultFormat = nullptr;
    bool m_scroll = false;
    bool m_scrolling = false;
    bool m_colorLines = true;
};
