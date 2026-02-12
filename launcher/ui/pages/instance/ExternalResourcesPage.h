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
#pragma once

#include <QMainWindow>
#include <QSortFilterProxyModel>

#include "Application.h"
#include "minecraft/MinecraftInstance.h"
#include "settings/Setting.h"
#include "ui/pages/BasePage.h"

class ResourceFolderModel;

namespace Ui
{
	class ExternalResourcesPage;
}

/* This page is used as a base for pages in which the user can manage external resources
 * related to the game, such as mods, shaders or resource packs. */
class ExternalResourcesPage : public QMainWindow, public BasePage
{
	Q_OBJECT

  public:
	explicit ExternalResourcesPage(BaseInstance* instance,
								   std::shared_ptr<ResourceFolderModel> model,
								   QWidget* parent = nullptr);
	virtual ~ExternalResourcesPage();

	virtual QString displayName() const override = 0;
	virtual QIcon icon() const override			 = 0;
	virtual QString id() const override			 = 0;
	virtual QString helpPage() const override	 = 0;

	virtual bool shouldDisplay() const override = 0;
	QString extraHeaderInfoString();

	void openedImpl() override;
	void closedImpl() override;

	void retranslate() override;

  protected:
	bool eventFilter(QObject* obj, QEvent* ev) override;
	bool listFilter(QKeyEvent* ev);
	QMenu* createPopupMenu() override;

  public slots:
	virtual void updateActions();
	virtual void updateFrame(const QModelIndex& current, const QModelIndex& previous);

  protected slots:
	void itemActivated(const QModelIndex& index);
	void filterTextChanged(const QString& newContents);

	virtual void addItem();
	void removeItem();
	virtual void removeItems(const QItemSelection& selection);

	virtual void enableItem();
	virtual void disableItem();

	virtual void viewHomepage();

	virtual void viewFolder();
	virtual void viewConfigs();

	void ShowContextMenu(const QPoint& pos);
	void ShowHeaderContextMenu(const QPoint& pos);

  protected:
	BaseInstance* m_instance = nullptr;

	Ui::ExternalResourcesPage* ui = nullptr;
	std::shared_ptr<ResourceFolderModel> m_model;
	QSortFilterProxyModel* m_filterModel = nullptr;

	QString m_fileSelectionFilter;
	QString m_viewFilter;

	std::shared_ptr<Setting> m_wide_bar_setting = nullptr;
};
