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

#include <QString>
#include <QUrl>
#include <QWidget>

#include "ui/widgets/HubViewBase.h"

class QLineEdit;
class QLabel;
class QVBoxLayout;
class QStackedWidget;
class QTabBar;
class QToolButton;
class QWidget;
class QPushButton;
class NewsChecker;
class QEvent;

class LauncherHubWidget : public QWidget
{
	Q_OBJECT

  public:
	explicit LauncherHubWidget(QWidget* parent = nullptr);
	~LauncherHubWidget() override;

	void ensureLoaded();
	void loadHome();
	void openUrl(const QUrl& url);
	void newTab(const QUrl& url = QUrl());
	void setHomeUrl(const QUrl& url);
	QUrl homeUrl() const;
	void setSelectedInstanceId(const QString& id);
	void refreshCockpit();

  signals:
	void selectInstanceRequested(const QString& instanceId);
	void launchInstanceRequested(const QString& instanceId);
	void editInstanceRequested(const QString& instanceId);
	void backupsRequested(const QString& instanceId);
	void openInstanceFolderRequested(const QString& instanceId);

  private:
	HubViewBase* currentView() const;
	HubViewBase* viewForTabIndex(int index) const;
	int tabIndexForView(const HubViewBase* view) const;
	HubViewBase* createTab(const QUrl& url, const QString& label = QString(), bool switchTo = true);
	void createCockpitTab();
	void switchToPage(QWidget* page);
	void activatePendingForPage(QWidget* page);
	void updateNavigationState();
	void updateTabPerformanceState();
	void syncTabsUi();
	void refreshToolbarIcons();
	void rebuildRecentInstances();
	void rebuildNewsFeed();
	void updateHero();
	QString activeInstanceId() const;

  protected:
	void changeEvent(QEvent* event) override;

	QTabBar* m_tabBar					 = nullptr;
	QWidget* m_tabsBarContainer			 = nullptr;
	QWidget* m_toolbarContainer			 = nullptr;
	QStackedWidget* m_stack				 = nullptr;
	QLineEdit* m_addressBar				 = nullptr;
	QToolButton* m_backButton			 = nullptr;
	QToolButton* m_forwardButton		 = nullptr;
	QToolButton* m_reloadButton			 = nullptr;
	QToolButton* m_homeButton			 = nullptr;
	QToolButton* m_goButton				 = nullptr;
	QToolButton* m_newTabButton			 = nullptr;
	QWidget* m_cockpitPage				 = nullptr;
	QLabel* m_cockpitBadgeLabel			 = nullptr;
	QLabel* m_cockpitTitleLabel			 = nullptr;
	QLabel* m_cockpitSubtitleLabel		 = nullptr;
	QLabel* m_cockpitIconLabel			 = nullptr;
	QPushButton* m_playButton			 = nullptr;
	QPushButton* m_editButton			 = nullptr;
	QPushButton* m_backupsButton		 = nullptr;
	QPushButton* m_folderButton			 = nullptr;
	QLabel* m_instancesValueLabel		 = nullptr;
	QLabel* m_instancesDetailLabel		 = nullptr;
	QLabel* m_playtimeValueLabel		 = nullptr;
	QLabel* m_playtimeDetailLabel		 = nullptr;
	QLabel* m_attentionValueLabel		 = nullptr;
	QLabel* m_attentionDetailLabel		 = nullptr;
	QVBoxLayout* m_recentInstancesLayout = nullptr;
	QVBoxLayout* m_newsLayout			 = nullptr;
	NewsChecker* m_newsChecker			 = nullptr;
	QString m_selectedInstanceId;
	QUrl m_homeUrl;
	bool m_loaded = false;
};
