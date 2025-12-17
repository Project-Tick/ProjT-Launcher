// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
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
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "NewsDialog.h"
#include "ui_NewsDialog.h"

NewsDialog::NewsDialog(QList<NewsEntryPtr> entries, QWidget* parent) : QDialog(parent), ui(new Ui::NewsDialog())
{
    ui->setupUi(this);

    for (auto entry : entries) {
        ui->articleListWidget->addItem(entry->title);
        m_entries.insert(entry->title, entry);
    }

    connect(ui->articleListWidget, &QListWidget::currentTextChanged, this, &NewsDialog::selectedArticleChanged);
    connect(ui->toggleListButton, &QPushButton::clicked, this, &NewsDialog::toggleArticleList);

    m_article_list_hidden = ui->articleListWidget->isHidden();

    auto first_item = ui->articleListWidget->item(0);
    first_item->setSelected(true);

    auto article_entry = m_entries.constFind(first_item->text()).value();
    ui->articleTitleLabel->setText(QString("<a href='%1'>%2</a>").arg(article_entry->link, first_item->text()));

    ui->currentArticleContentBrowser->setText(article_entry->content);
    ui->currentArticleContentBrowser->flush();
}

NewsDialog::~NewsDialog()
{
    delete ui;
}

void NewsDialog::selectedArticleChanged(const QString& new_title)
{
    auto article_entry = m_entries.constFind(new_title).value();

    ui->articleTitleLabel->setText(QString("<a href='%1'>%2</a>").arg(article_entry->link, new_title));

    ui->currentArticleContentBrowser->setText(article_entry->content);
    ui->currentArticleContentBrowser->flush();
}

void NewsDialog::toggleArticleList()
{
    m_article_list_hidden = !m_article_list_hidden;

    ui->articleListWidget->setHidden(m_article_list_hidden);

    if (m_article_list_hidden)
        ui->toggleListButton->setText(tr("Show article list"));
    else
        ui->toggleListButton->setText(tr("Hide article list"));
}
