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

#include "QtWebEngineHubView.h"

#if defined(PROJT_USE_WEBENGINE)

#include <QApplication>
#include <QDir>
#include <QStandardPaths>
#include <QVBoxLayout>
#include <QWebChannel>
#include <QWebEngineHistory>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QWebEngineSettings>

#include "BuildConfig.h"

namespace
{
	class LauncherHubBridge final : public QObject
	{
		Q_OBJECT
		Q_PROPERTY(QString launcherVersion READ launcherVersion CONSTANT)

	  public:
		explicit LauncherHubBridge(QObject* parent = nullptr) : QObject(parent)
		{}

		QString launcherVersion() const
		{
			return BuildConfig.printableVersionString();
		}
	};

	class LauncherHubPage final : public QWebEnginePage
	{
	  public:
		LauncherHubPage(QWebEngineProfile* profile, QObject* parent = nullptr) : QWebEnginePage(profile, parent)
		{}

	  protected:
		bool acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame) override
		{
			Q_UNUSED(url);
			Q_UNUSED(type);
			Q_UNUSED(isMainFrame);
			return true;
		}
	};

	QWebEngineProfile* sharedHubProfile()
	{
		static QWebEngineProfile* sharedProfile = nullptr;
		if (!sharedProfile)
		{
			sharedProfile = new QWebEngineProfile(QStringLiteral("LauncherHub"), qApp);
			sharedProfile->setPersistentCookiesPolicy(QWebEngineProfile::AllowPersistentCookies);
			sharedProfile->setHttpCacheType(QWebEngineProfile::DiskHttpCache);
			sharedProfile->setHttpCacheMaximumSize(256 * 1024 * 1024);
			const QString storageRoot =
				QDir::cleanPath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/webengine");
			QDir().mkpath(storageRoot);
			sharedProfile->setPersistentStoragePath(storageRoot + "/storage");
			sharedProfile->setCachePath(storageRoot + "/cache");
		}
		return sharedProfile;
	}
}

QtWebEngineHubView::QtWebEngineHubView(QWidget* parent) : HubViewBase(parent)
{
	auto* layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);

	m_view = new QWebEngineView(this);
	layout->addWidget(m_view);

	auto* page = new LauncherHubPage(sharedHubProfile(), m_view);
	m_view->setPage(page);
	m_view->setAttribute(Qt::WA_OpaquePaintEvent, true);
	m_view->setStyleSheet(QStringLiteral("background: #121822;"));
	page->setBackgroundColor(QColor(QStringLiteral("#121822")));
	m_view->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
	m_view->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, false);
	m_view->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, false);
	m_view->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, false);
	m_view->settings()->setAttribute(QWebEngineSettings::HyperlinkAuditingEnabled, false);
	m_view->settings()->setAttribute(QWebEngineSettings::ScrollAnimatorEnabled, false);
	m_view->settings()->setAttribute(QWebEngineSettings::WebGLEnabled, false);
	m_view->settings()->setAttribute(QWebEngineSettings::Accelerated2dCanvasEnabled, false);

	auto* channel = new QWebChannel(m_view);
	auto* bridge  = new LauncherHubBridge(channel);
	channel->registerObject(QStringLiteral("launcher"), bridge);
	page->setWebChannel(channel);

	connect(m_view, &QWebEngineView::titleChanged, this, &QtWebEngineHubView::titleChanged);
	connect(m_view, &QWebEngineView::urlChanged, this, &QtWebEngineHubView::urlChanged);
	connect(m_view, &QWebEngineView::loadFinished, this, &QtWebEngineHubView::loadFinished);
	connect(m_view, &QWebEngineView::urlChanged, this, [this](const QUrl&) { emit navigationStateChanged(); });
	connect(m_view, &QWebEngineView::loadFinished, this, [this](bool) { emit navigationStateChanged(); });
}

QtWebEngineHubView::~QtWebEngineHubView() = default;

void QtWebEngineHubView::setUrl(const QUrl& url)
{
	if (m_view)
	{
		m_view->setUrl(url);
	}
}

QUrl QtWebEngineHubView::url() const
{
	return m_view ? m_view->url() : QUrl();
}

bool QtWebEngineHubView::canGoBack() const
{
	return m_view && m_view->history() ? m_view->history()->canGoBack() : false;
}

bool QtWebEngineHubView::canGoForward() const
{
	return m_view && m_view->history() ? m_view->history()->canGoForward() : false;
}

void QtWebEngineHubView::setActive(bool active)
{
	if (!m_view || !m_view->page())
	{
		return;
	}

	m_view->page()->setLifecycleState(active ? QWebEnginePage::LifecycleState::Active
											 : QWebEnginePage::LifecycleState::Frozen);
}

void QtWebEngineHubView::back()
{
	if (m_view)
	{
		m_view->back();
	}
}

void QtWebEngineHubView::forward()
{
	if (m_view)
	{
		m_view->forward();
	}
}

void QtWebEngineHubView::reload()
{
	if (m_view)
	{
		m_view->reload();
	}
}

#include "QtWebEngineHubView.moc"

#endif
