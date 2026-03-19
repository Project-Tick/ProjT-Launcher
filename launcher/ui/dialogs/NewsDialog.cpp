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
#include "NewsDialog.h"

#include "Application.h"
#include "settings/SettingsObject.h"

#include "ui_NewsDialog.h"

#include <QUrl>

NewsDialog::NewsDialog(QList<NewsEntryPtr> entries, QWidget* parent) : QDialog(parent), ui(new Ui::NewsDialog())
{
	ui->setupUi(this);

	for (auto entry : entries)
	{
		ui->articleListWidget->addItem(entry->title);
		m_entries.insert(entry->title, entry);
	}

	connect(ui->articleListWidget, &QListWidget::currentTextChanged, this, &NewsDialog::selectedArticleChanged);
	connect(ui->toggleListButton, &QPushButton::clicked, this, &NewsDialog::toggleArticleList);
	connect(ui->openInHubButton,
			&QPushButton::clicked,
			this,
			[this]()
			{
				if (m_current_link.isEmpty())
					return;
				emit openHubRequested(QUrl(m_current_link));
				accept();
			});
#if defined(PROJT_DISABLE_LAUNCHER_HUB)
	ui->openInHubButton->setEnabled(false);
	ui->openInHubButton->setToolTip(tr("Launcher Hub is not available in this build."));
#endif

	m_article_list_hidden = ui->articleListWidget->isHidden();

	auto first_item = ui->articleListWidget->item(0);
	if (!first_item)
		return;
	first_item->setSelected(true);

	auto article_it = m_entries.constFind(first_item->text());
	if (article_it == m_entries.cend() || !article_it.value())
		return;
	auto article_entry = article_it.value();
	ui->articleTitleLabel->setText(QString("<a href='%1'>%2</a>").arg(article_entry->link, first_item->text()));
	m_current_link = article_entry->link;
#if !defined(PROJT_DISABLE_LAUNCHER_HUB)
	ui->openInHubButton->setEnabled(!m_current_link.isEmpty());
#endif

	ui->currentArticleContentBrowser->setText(article_entry->content);
	ui->currentArticleContentBrowser->flush();

	connect(this, &QDialog::finished, this, [this] {
		APPLICATION->settings()->set("NewsGeometry", QString::fromUtf8(saveGeometry().toBase64()));
	});
	const QByteArray base64Geometry = APPLICATION->settings()->get("NewsGeometry").toString().toUtf8();
	restoreGeometry(QByteArray::fromBase64(base64Geometry));
}

NewsDialog::~NewsDialog()
{
	delete ui;
}

void NewsDialog::selectedArticleChanged(const QString& new_title)
{
	auto article_it = m_entries.constFind(new_title);
	if (article_it == m_entries.cend() || !article_it.value())
		return;
	auto article_entry = article_it.value();

	ui->articleTitleLabel->setText(QString("<a href='%1'>%2</a>").arg(article_entry->link, new_title));
	m_current_link = article_entry->link;
#if !defined(PROJT_DISABLE_LAUNCHER_HUB)
	ui->openInHubButton->setEnabled(!m_current_link.isEmpty());
#endif

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
