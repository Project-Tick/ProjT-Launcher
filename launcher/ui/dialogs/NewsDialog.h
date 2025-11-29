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

#include <QDialog>

class NewsViewModel;

namespace Ui {
class NewsDialog;
}

class NewsDialog : public QDialog {
    Q_OBJECT

   public:
    NewsDialog(NewsViewModel* viewModel, QWidget* parent = nullptr);
    ~NewsDialog();

   public slots:
    void toggleArticleList();

   private slots:
    void selectedArticleChanged(const QString& new_title);

   private:
    void refreshArticles();
    void applyCurrentArticle();

    Ui::NewsDialog* ui;

    NewsViewModel* m_viewModel = nullptr;
    bool m_article_list_hidden = false;
};
