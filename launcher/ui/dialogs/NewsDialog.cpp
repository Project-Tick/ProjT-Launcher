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
#include "NewsDialog.h"
#include "ui_NewsDialog.h"

#include <QSignalBlocker>

#include "viewmodels/NewsViewModel.h"

NewsDialog::NewsDialog(NewsViewModel* viewModel, QWidget* parent)
    : QDialog(parent), ui(new Ui::NewsDialog()), m_viewModel(viewModel)
{
    ui->setupUi(this);

    connect(ui->articleListWidget, &QListWidget::currentTextChanged, this, &NewsDialog::selectedArticleChanged);
    connect(ui->toggleListButton, &QPushButton::clicked, this, &NewsDialog::toggleArticleList);

    m_article_list_hidden = ui->articleListWidget->isHidden();

    if (m_viewModel) {
        connect(m_viewModel, &NewsViewModel::newsUpdated, this, &NewsDialog::refreshArticles);
        connect(m_viewModel, &NewsViewModel::currentContentChanged, this, &NewsDialog::applyCurrentArticle);
        refreshArticles();
        applyCurrentArticle();
    } else {
        ui->articleListWidget->clear();
    }
}

NewsDialog::~NewsDialog()
{
    delete ui;
}

void NewsDialog::selectedArticleChanged(const QString& new_title)
{
    if (!m_viewModel || new_title.isEmpty()) {
        return;
    }
    m_viewModel->selectArticle(new_title);
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

void NewsDialog::refreshArticles()
{
    if (!m_viewModel) {
        return;
    }
    const auto entries = m_viewModel->entries();
    QString fallbackTitle;
    for (const auto& entry : entries) {
        if (entry) {
            fallbackTitle = entry->title;
            break;
        }
    }
    const QString desiredTitle =
        !m_viewModel->currentTitle().isEmpty() ? m_viewModel->currentTitle() : fallbackTitle;
    const QSignalBlocker blocker(ui->articleListWidget);
    ui->articleListWidget->clear();
    for (const auto& entry : entries) {
        if (!entry) {
            continue;
        }
        ui->articleListWidget->addItem(entry->title);
    }
    if (desiredTitle.isEmpty()) {
        return;
    }
    const auto matches = ui->articleListWidget->findItems(desiredTitle, Qt::MatchExactly);
    if (!matches.isEmpty()) {
        ui->articleListWidget->setCurrentItem(matches.constFirst());
    } else if (ui->articleListWidget->count() > 0) {
        ui->articleListWidget->setCurrentRow(0);
        auto currentItem = ui->articleListWidget->currentItem();
        if (currentItem) {
            m_viewModel->selectArticle(currentItem->text());
        }
    }
}

void NewsDialog::applyCurrentArticle()
{
    if (!m_viewModel) {
        ui->articleTitleLabel->clear();
        ui->currentArticleContentBrowser->clear();
        return;
    }
    const QString title = m_viewModel->currentTitle();
    const QString link = m_viewModel->currentLink();
    if (title.isEmpty()) {
        ui->articleTitleLabel->clear();
        ui->currentArticleContentBrowser->clear();
        return;
    }
    ui->articleTitleLabel->setText(QString("<a href='%1'>%2</a>").arg(link, title));
    ui->currentArticleContentBrowser->setText(m_viewModel->currentContent());
    ui->currentArticleContentBrowser->flush();
}
