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

#include "LauncherHubWidget.h"

#include <QDesktopServices>
#include <QDir>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QStandardPaths>
#include <QStackedWidget>
#include <QTabBar>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWebEngineHistory>
#include <QWebChannel>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QWebEngineView>

#include "BuildConfig.h"

namespace
{
QUrl defaultHubUrl()
{
	if (!BuildConfig.HUB_HOME_URL.isEmpty())
	{
		return QUrl(BuildConfig.HUB_HOME_URL);
	}
	return QUrl(QStringLiteral("https://projecttick.org/p/projt-launcher/"));
}

	QUrl resolveInput(const QString& input)
	{
		const QString trimmed = input.trimmed();
		if (trimmed.isEmpty())
		{
			return {};
		}

		QUrl url = QUrl::fromUserInput(trimmed);
		if (url.isValid() && !url.scheme().isEmpty())
		{
			return url;
		}

	const QString templateUrl = BuildConfig.HUB_SEARCH_URL;
	if (templateUrl.contains("%1"))
	{
		const QByteArray encoded = QUrl::toPercentEncoding(trimmed);
		return QUrl(templateUrl.arg(QString::fromUtf8(encoded)));
	}
	QUrl fallback(templateUrl);
	if (fallback.isValid() && !fallback.scheme().isEmpty())
	{
		return fallback;
	}
	return QUrl(QStringLiteral("https://www.google.com/search?q=%1").arg(QString::fromUtf8(QUrl::toPercentEncoding(trimmed))));
}
}

class LauncherHubBridge final : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString launcherVersion READ launcherVersion CONSTANT)

  public:
	explicit LauncherHubBridge(QObject* parent = nullptr) : QObject(parent) {}

	QString launcherVersion() const
	{
		return BuildConfig.printableVersionString();
	}

	Q_INVOKABLE void openExternal(const QString& url) const
	{
		QDesktopServices::openUrl(QUrl(url));
	}
};

class LauncherHubPage final : public QWebEnginePage
{
  public:
	LauncherHubPage(QWebEngineProfile* profile, QObject* parent = nullptr)
		: QWebEnginePage(profile, parent)
	{
	}

  protected:
	bool acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame) override
	{
		Q_UNUSED(url);
		Q_UNUSED(type);
		Q_UNUSED(isMainFrame);
		return true;
	}
};

LauncherHubWidget::LauncherHubWidget(QWidget* parent)
	: QWidget(parent)
{
	m_homeUrl = defaultHubUrl();

	auto* layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);

	auto* tabsLayout = new QHBoxLayout();
	tabsLayout->setContentsMargins(6, 6, 6, 0);

	m_tabBar = new QTabBar(this);
	m_tabBar->setMovable(true);
	m_tabBar->setExpanding(false);
	m_tabBar->setDocumentMode(true);
	m_tabBar->setTabsClosable(true);

	m_newTabButton = new QToolButton(this);
	m_newTabButton->setIcon(QIcon::fromTheme("list-add"));
	m_newTabButton->setToolTip(tr("New Tab"));

	tabsLayout->addWidget(m_tabBar, 1);
	tabsLayout->addWidget(m_newTabButton);

	auto* toolbar = new QHBoxLayout();
	toolbar->setContentsMargins(6, 6, 6, 6);

	m_backButton = new QToolButton(this);
	m_backButton->setIcon(QIcon::fromTheme("go-previous"));
	m_backButton->setToolTip(tr("Back"));
	m_backButton->setEnabled(false);

	m_forwardButton = new QToolButton(this);
	m_forwardButton->setIcon(QIcon::fromTheme("go-next"));
	m_forwardButton->setToolTip(tr("Forward"));
	m_forwardButton->setEnabled(false);

	m_reloadButton = new QToolButton(this);
	m_reloadButton->setIcon(QIcon::fromTheme("view-refresh"));
	m_reloadButton->setToolTip(tr("Reload"));

	m_homeButton = new QToolButton(this);
	m_homeButton->setIcon(QIcon::fromTheme("go-home"));
	m_homeButton->setToolTip(tr("Home"));

	m_addressBar = new QLineEdit(this);
	m_addressBar->setPlaceholderText(tr("Search or enter address"));
	m_addressBar->setClearButtonEnabled(true);

	m_goButton = new QToolButton(this);
	m_goButton->setIcon(QIcon::fromTheme("system-search"));
	m_goButton->setToolTip(tr("Go"));

	toolbar->addWidget(m_backButton);
	toolbar->addWidget(m_forwardButton);
	toolbar->addWidget(m_reloadButton);
	toolbar->addWidget(m_homeButton);
	toolbar->addWidget(m_addressBar, 1);
	toolbar->addWidget(m_goButton);

	m_profile = new QWebEngineProfile(QStringLiteral("LauncherHub"), this);
	m_profile->setPersistentCookiesPolicy(QWebEngineProfile::AllowPersistentCookies);
	const QString storageRoot =
		QDir::cleanPath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/webengine");
	QDir().mkpath(storageRoot);
	m_profile->setPersistentStoragePath(storageRoot + "/storage");
	m_profile->setCachePath(storageRoot + "/cache");

	m_stack = new QStackedWidget(this);

	layout->addLayout(tabsLayout);
	layout->addLayout(toolbar);
	layout->addWidget(m_stack);

	connect(m_backButton, &QToolButton::clicked, this, [this]()
			{
				if (auto* view = currentView())
				{
					view->back();
				}
			});
	connect(m_forwardButton, &QToolButton::clicked, this, [this]()
			{
				if (auto* view = currentView())
				{
					view->forward();
				}
			});
	connect(m_reloadButton, &QToolButton::clicked, this, [this]()
			{
				if (auto* view = currentView())
				{
					view->reload();
				}
			});
	connect(m_homeButton, &QToolButton::clicked, this, &LauncherHubWidget::loadHome);

	connect(m_goButton, &QToolButton::clicked, this, [this]()
			{
				openUrl(resolveInput(m_addressBar->text()));
			});
	connect(m_addressBar, &QLineEdit::returnPressed, this, [this]()
			{
				openUrl(resolveInput(m_addressBar->text()));
			});

	connect(m_newTabButton, &QToolButton::clicked, this, [this]()
			{
				newTab(m_homeUrl);
			});
	connect(m_tabBar, &QTabBar::currentChanged, this, [this](int index)
			{
				if (index >= 0 && index < m_stack->count())
				{
					m_stack->setCurrentIndex(index);
					updateNavigationState();
				}
			});
	connect(m_tabBar, &QTabBar::tabCloseRequested, this, [this](int index)
			{
				if (index < 0 || index >= m_stack->count())
				{
					return;
				}
				if (m_tabBar->count() == 1)
				{
					if (auto* view = qobject_cast<QWebEngineView*>(m_stack->widget(index)))
					{
						view->setUrl(m_homeUrl);
						m_tabBar->setTabText(index, tr("Home"));
						updateNavigationState();
					}
					return;
				}
				QWidget* widget = m_stack->widget(index);
				m_stack->removeWidget(widget);
				m_tabBar->removeTab(index);
				widget->deleteLater();

				const int newIndex = qMin(index, m_tabBar->count() - 1);
				m_tabBar->setCurrentIndex(newIndex);
				m_stack->setCurrentIndex(newIndex);
				updateNavigationState();
			});

	createTab(m_homeUrl, tr("Home"), true);
	createTab(QUrl(BuildConfig.NEWS_OPEN_URL), tr("News"), false);
	if (!BuildConfig.HUB_COMMUNITY_URL.isEmpty())
	{
		createTab(QUrl(BuildConfig.HUB_COMMUNITY_URL), tr("Community"), false);
	}
	createTab(QUrl(BuildConfig.HELP_URL.arg("")), tr("Help"), false);
}

LauncherHubWidget::~LauncherHubWidget() = default;

QWebEngineView* LauncherHubWidget::currentView() const
{
	if (!m_stack)
	{
		return nullptr;
	}
	return qobject_cast<QWebEngineView*>(m_stack->currentWidget());
}

QWebEngineView* LauncherHubWidget::createTab(const QUrl& url, const QString& label, bool switchTo)
{
	if (!m_stack || !m_tabBar)
	{
		return nullptr;
	}

	auto* view = new QWebEngineView(m_stack);
	auto* page = new LauncherHubPage(m_profile, view);
	view->setPage(page);
	view->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
	view->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, false);
	view->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, false);

	auto* channel = new QWebChannel(view);
	auto* bridge = new LauncherHubBridge(channel);
	channel->registerObject(QStringLiteral("launcher"), bridge);
	page->setWebChannel(channel);

	const int stackIndex = m_stack->addWidget(view);
	const QString initialLabel = label.isEmpty() ? tr("New Tab") : label;
	m_tabBar->addTab(initialLabel);

	connect(view, &QWebEngineView::titleChanged, this, [this, view](const QString& title)
			{
				const int index = m_stack->indexOf(view);
				if (index >= 0)
				{
					if (!title.isEmpty())
					{
						m_tabBar->setTabText(index, title);
					}
				}
			});
	connect(view, &QWebEngineView::urlChanged, this, [this, view](const QUrl& urlChanged)
			{
				if (view == currentView())
				{
					m_addressBar->setText(urlChanged.toString());
					updateNavigationState();
				}
			});
	connect(view, &QWebEngineView::loadFinished, this, [this, view](bool)
			{
				if (view == currentView())
				{
					updateNavigationState();
				}
			});

	if (switchTo)
	{
		m_tabBar->setCurrentIndex(stackIndex);
		m_stack->setCurrentIndex(stackIndex);
	}

	if (url.isValid())
	{
		view->setUrl(url);
	}

	return view;
}

void LauncherHubWidget::updateNavigationState()
{
	auto* view = currentView();
	if (!view)
	{
		m_backButton->setEnabled(false);
		m_forwardButton->setEnabled(false);
		m_addressBar->clear();
		return;
	}
	m_backButton->setEnabled(view->history()->canGoBack());
	m_forwardButton->setEnabled(view->history()->canGoForward());
	m_addressBar->setText(view->url().toString());
}

void LauncherHubWidget::ensureLoaded()
{
	if (m_loaded)
	{
		return;
	}
	loadHome();
	m_loaded = true;
}

void LauncherHubWidget::loadHome()
{
	openUrl(m_homeUrl);
}

void LauncherHubWidget::newTab(const QUrl& url)
{
	createTab(url.isValid() ? url : m_homeUrl, QString(), true);
	m_loaded = true;
}

void LauncherHubWidget::openUrl(const QUrl& url)
{
	auto* view = currentView();
	if (!view)
	{
		view = createTab(m_homeUrl, QString(), true);
	}
	if (!view || !url.isValid())
	{
		return;
	}
	view->setUrl(url);
	m_loaded = true;
}

void LauncherHubWidget::setHomeUrl(const QUrl& url)
{
	m_homeUrl = url;
	m_loaded = false;
}

QUrl LauncherHubWidget::homeUrl() const
{
	return m_homeUrl;
}

#include "LauncherHubWidget.moc"
