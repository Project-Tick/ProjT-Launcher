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
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#pragma once

#include <QUrl>
#include <QWidget>

#if defined(PROJT_USE_WEBENGINE)
#include <QWebEngineView>
#endif

#if defined(PROJT_USE_WEBVIEW2)
#include "ui/widgets/WebView2Widget.h"
#endif

class QLineEdit;
class QStackedWidget;
class QTabBar;
class QToolButton;
class QWebEngineProfile;
class QWidget;

#if !defined(PROJT_DISABLE_LAUNCHER_HUB)
#if defined(PROJT_USE_WEBENGINE)
using HubView = QWebEngineView;
#elif defined(PROJT_USE_WEBVIEW2)
using HubView = WebView2Widget;
#endif
#endif

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

  private:
	HubView* currentView() const;
	HubView* createTab(const QUrl& url, const QString& label = QString(), bool switchTo = true);
	void updateNavigationState();

	QTabBar* m_tabBar			 = nullptr;
	QWidget* m_tabsBarContainer	 = nullptr;
	QWidget* m_toolbarContainer	 = nullptr;
	QStackedWidget* m_stack		 = nullptr;
	QWebEngineProfile* m_profile = nullptr;
	QLineEdit* m_addressBar		 = nullptr;
	QToolButton* m_backButton	 = nullptr;
	QToolButton* m_forwardButton = nullptr;
	QToolButton* m_reloadButton	 = nullptr;
	QToolButton* m_homeButton	 = nullptr;
	QToolButton* m_goButton		 = nullptr;
	QToolButton* m_newTabButton	 = nullptr;
	QUrl m_homeUrl;
	bool m_loaded = false;
};
