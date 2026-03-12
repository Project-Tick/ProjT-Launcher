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
 *
 * === Upstream License Block (Do Not Modify) ==============================
 *
 *
 *
 *
 *
 *
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (C) 2023 Rachel Powers <508861+Ryex@users.noreply.github.com>
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
 *
 *
 *
 * ======================================================================== */

#include "UpdaterDialogs.h"

#include "ui_SelectReleaseDialog.h"

#include <QPushButton>
#include <QTextBrowser>
#include "Markdown.h"
#include "StringUtils.h"

SelectReleaseDialog::SelectReleaseDialog(const Version& current_version,
										 const QList<ReleaseInfo>& releases,
										 QWidget* parent)
	: QDialog(parent),
	  m_releases(releases),
	  m_currentVersion(current_version),
	  ui(new Ui::SelectReleaseDialog)
{
	ui->setupUi(this);

	ui->changelogTextBrowser->setOpenExternalLinks(true);
	ui->changelogTextBrowser->setLineWrapMode(QTextBrowser::LineWrapMode::WidgetWidth);
	ui->changelogTextBrowser->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);

	ui->versionsTree->setColumnCount(2);

	ui->versionsTree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
	ui->versionsTree->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
	ui->versionsTree->setHeaderLabels({ tr("Version"), tr("Published Date") });
	ui->versionsTree->header()->setStretchLastSection(false);

	ui->eplainLabel->setText(tr("Select a version to install.\n"
								"\n"
								"Currently installed version: %1")
								 .arg(m_currentVersion.toString()));

	loadReleases();

	connect(ui->versionsTree, &QTreeWidget::currentItemChanged, this, &SelectReleaseDialog::selectionChanged);

	connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &SelectReleaseDialog::accept);
	connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &SelectReleaseDialog::reject);

	ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
	ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
}

SelectReleaseDialog::~SelectReleaseDialog()
{
	delete ui;
}

void SelectReleaseDialog::loadReleases()
{
	for (auto rls : m_releases)
	{
		appendRelease(rls);
	}
}

void SelectReleaseDialog::appendRelease(ReleaseInfo const& release)
{
	auto rls_item = new QTreeWidgetItem(ui->versionsTree);
	rls_item->setText(0, release.tag_name);
	rls_item->setExpanded(true);
	rls_item->setText(1, release.published_at.toString());
	rls_item->setData(0, Qt::UserRole, QVariant(release.tag_name));

	ui->versionsTree->addTopLevelItem(rls_item);
}

ReleaseInfo SelectReleaseDialog::getRelease(QTreeWidgetItem* item)
{
	auto tag_name = item->data(0, Qt::UserRole).toString();
	ReleaseInfo release;
	for (auto rls : m_releases)
	{
		if (rls.tag_name == tag_name)
			release = rls;
	}
	return release;
}

void SelectReleaseDialog::selectionChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
	Q_UNUSED(previous)
	ReleaseInfo release = getRelease(current);
	QString body		= markdownToHTML(release.body.toUtf8());
	m_selectedRelease	= release;

	ui->changelogTextBrowser->setHtml(StringUtils::htmlListPatch(body));
}

SelectReleaseAssetDialog::SelectReleaseAssetDialog(const QList<ReleaseAsset>& assets, QWidget* parent)
	: QDialog(parent),
	  m_assets(assets),
	  ui(new Ui::SelectReleaseDialog)
{
	ui->setupUi(this);

	ui->changelogTextBrowser->setOpenExternalLinks(true);
	ui->changelogTextBrowser->setLineWrapMode(QTextBrowser::LineWrapMode::WidgetWidth);
	ui->changelogTextBrowser->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);

	ui->versionsTree->setColumnCount(2);

	ui->versionsTree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
	ui->versionsTree->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
	ui->versionsTree->setHeaderLabels({ tr("Version"), tr("Published Date") });
	ui->versionsTree->header()->setStretchLastSection(false);

	ui->eplainLabel->setText(tr("Select a version to install."));

	ui->changelogTextBrowser->setHidden(true);

	loadAssets();

	connect(ui->versionsTree, &QTreeWidget::currentItemChanged, this, &SelectReleaseAssetDialog::selectionChanged);

	connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &SelectReleaseAssetDialog::accept);
	connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &SelectReleaseAssetDialog::reject);
}

SelectReleaseAssetDialog::~SelectReleaseAssetDialog()
{
	delete ui;
}

void SelectReleaseAssetDialog::loadAssets()
{
	for (auto rls : m_assets)
	{
		appendAsset(rls);
	}
}

void SelectReleaseAssetDialog::appendAsset(ReleaseAsset const& asset)
{
	auto rls_item = new QTreeWidgetItem(ui->versionsTree);
	rls_item->setText(0, asset.name);
	rls_item->setExpanded(true);
	rls_item->setText(1, asset.updated_at.toString());
	rls_item->setData(0, Qt::UserRole, QVariant(asset.download_url.toString()));

	ui->versionsTree->addTopLevelItem(rls_item);
}

ReleaseAsset SelectReleaseAssetDialog::getAsset(QTreeWidgetItem* item)
{
	auto asset_url = item->data(0, Qt::UserRole).toString();
	ReleaseAsset selected_asset;
	for (auto asset : m_assets)
	{
		if (asset.download_url.toString() == asset_url)
			selected_asset = asset;
	}
	return selected_asset;
}

void SelectReleaseAssetDialog::selectionChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
	Q_UNUSED(previous)
	ReleaseAsset asset = getAsset(current);
	m_selectedAsset	   = asset;
}
