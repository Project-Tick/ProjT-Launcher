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

#include "LauncherHubWidget.h"

#include <QApplication>
#include <QDateTime>
#include <QDesktopServices>
#include <QDir>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QLocale>
#include <QPushButton>
#include <QScrollArea>
#include <QSizePolicy>
#include <QStandardPaths>
#include <QStackedWidget>
#include <QTabBar>
#include <QTextDocumentFragment>
#include <QToolButton>
#include <QVBoxLayout>
#if defined(PROJT_USE_WEBENGINE)
#include <QWebChannel>
#include <QWebEngineHistory>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QWebEngineView>
#endif

#if defined(PROJT_USE_WEBVIEW2)
#include "ui/widgets/WebView2Widget.h"
#endif

#include "Application.h"
#include "BaseInstance.h"
#include "BuildConfig.h"
#include "InstanceList.h"
#include "MMCTime.h"
#include "icons/IconList.hpp"
#include "news/NewsChecker.h"

#if defined(PROJT_DISABLE_LAUNCHER_HUB)
LauncherHubWidget::LauncherHubWidget(QWidget* parent) : QWidget(parent)
{
	auto* layout = new QVBoxLayout(this);
	layout->setContentsMargins(24, 24, 24, 24);

	auto* label = new QLabel(tr("Launcher Hub is not available in this build."), this);
	label->setAlignment(Qt::AlignCenter);
	label->setWordWrap(true);
	layout->addWidget(label, 1);
}

LauncherHubWidget::~LauncherHubWidget() = default;

void LauncherHubWidget::ensureLoaded()
{}

void LauncherHubWidget::loadHome()
{}

void LauncherHubWidget::openUrl(const QUrl& url)
{
	if (url.isValid())
		QDesktopServices::openUrl(url);
}

void LauncherHubWidget::newTab(const QUrl& url)
{
	openUrl(url);
}

void LauncherHubWidget::setHomeUrl(const QUrl& url)
{
	m_homeUrl = url;
}

QUrl LauncherHubWidget::homeUrl() const
{
	return m_homeUrl;
}

void LauncherHubWidget::setSelectedInstanceId(const QString&)
{}

void LauncherHubWidget::refreshCockpit()
{}

#else

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
		return QUrl(QStringLiteral("https://www.google.com/search?q=%1")
						.arg(QString::fromUtf8(QUrl::toPercentEncoding(trimmed))));
	}

	void clearLayout(QLayout* layout)
	{
		if (!layout)
		{
			return;
		}

		while (auto* item = layout->takeAt(0))
		{
			if (auto* widget = item->widget())
			{
				widget->deleteLater();
			}
			if (auto* childLayout = item->layout())
			{
				clearLayout(childLayout);
				delete childLayout;
			}
			delete item;
		}
	}

	QString relativeTimeLabel(qint64 timestamp)
	{
		if (timestamp <= 0)
		{
			return LauncherHubWidget::tr("Never launched");
		}

		const QDateTime launchedAt = QDateTime::fromMSecsSinceEpoch(timestamp);
		const qint64 secondsAgo	   = launchedAt.secsTo(QDateTime::currentDateTime());
		if (secondsAgo < 60)
		{
			return LauncherHubWidget::tr("Just now");
		}
		if (secondsAgo < 3600)
		{
			return LauncherHubWidget::tr("%1 min ago").arg(secondsAgo / 60);
		}
		if (secondsAgo < 86400)
		{
			return LauncherHubWidget::tr("%1 hr ago").arg(secondsAgo / 3600);
		}
		if (secondsAgo < 604800)
		{
			return LauncherHubWidget::tr("%1 day(s) ago").arg(secondsAgo / 86400);
		}
		return QLocale().toString(launchedAt, QLocale::ShortFormat);
	}

	QString stripHtmlExcerpt(const QString& html, int maxLength = 120)
	{
		QString text = QTextDocumentFragment::fromHtml(html).toPlainText().simplified();
		if (text.size() <= maxLength)
		{
			return text;
		}
		return text.left(maxLength - 1) + QStringLiteral("...");
	}

	QString heroBadgeForInstance(const InstancePtr& instance)
	{
		if (!instance)
		{
			return LauncherHubWidget::tr("Cockpit");
		}
		if (instance->isRunning())
		{
			return LauncherHubWidget::tr("Now playing");
		}
		if (instance->hasCrashed() || instance->hasVersionBroken())
		{
			return LauncherHubWidget::tr("Needs attention");
		}
		if (instance->hasUpdateAvailable())
		{
			return LauncherHubWidget::tr("Update ready");
		}
		return LauncherHubWidget::tr("Ready to launch");
	}

	QList<InstancePtr> sortedInstances()
	{
		QList<InstancePtr> instances;
		if (!APPLICATION->instances())
		{
			return instances;
		}

		for (int i = 0; i < APPLICATION->instances()->count(); ++i)
		{
			instances.append(APPLICATION->instances()->at(i));
		}

		std::sort(instances.begin(),
				  instances.end(),
				  [](const InstancePtr& left, const InstancePtr& right)
				  {
					  if (left->lastLaunch() == right->lastLaunch())
					  {
						  return left->name().localeAwareCompare(right->name()) < 0;
					  }
					  return left->lastLaunch() > right->lastLaunch();
				  });
		return instances;
	}
}

#if !defined(PROJT_USE_WEBENGINE) && !defined(PROJT_USE_WEBVIEW2)
class HubView final : public QWidget
{
	Q_OBJECT

  public:
	explicit HubView(QWidget* parent = nullptr) : QWidget(parent)
	{
		auto* layout = new QVBoxLayout(this);
		layout->setContentsMargins(24, 24, 24, 24);
		layout->setSpacing(12);

		auto* titleLabel = new QLabel(LauncherHubWidget::tr("This page opens in your browser on this platform."), this);
		titleLabel->setWordWrap(true);
		titleLabel->setStyleSheet(QStringLiteral("color: #ffffff; font-size: 18px; font-weight: 700;"));

		m_urlLabel = new QLabel(this);
		m_urlLabel->setWordWrap(true);
		m_urlLabel->setStyleSheet(QStringLiteral("color: #9bb0cc;"));

		m_openButton = new QPushButton(LauncherHubWidget::tr("Open in browser"), this);
		connect(m_openButton,
				&QPushButton::clicked,
				this,
				[this]()
				{
					if (m_url.isValid())
					{
						QDesktopServices::openUrl(m_url);
					}
				});

		layout->addWidget(titleLabel);
		layout->addWidget(m_urlLabel);
		layout->addWidget(m_openButton, 0, Qt::AlignLeft);
		layout->addStretch(1);
	}

	void setUrl(const QUrl& url)
	{
		m_url = url;
		m_urlLabel->setText(url.toString());
		m_openButton->setEnabled(url.isValid());
		if (url.isValid())
		{
			QDesktopServices::openUrl(url);
		}
	}

	QUrl url() const
	{
		return m_url;
	}

	void back()
	{}

	void forward()
	{}

	void reload()
	{
		if (m_url.isValid())
		{
			QDesktopServices::openUrl(m_url);
		}
	}

	bool canGoBack() const
	{
		return false;
	}

	bool canGoForward() const
	{
		return false;
	}

  private:
	QUrl m_url;
	QLabel* m_urlLabel		  = nullptr;
	QPushButton* m_openButton = nullptr;
};
#endif

#if defined(PROJT_USE_WEBENGINE)
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

	Q_INVOKABLE void openExternal(const QString& url) const
	{
		QDesktopServices::openUrl(QUrl(url));
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
#endif

LauncherHubWidget::LauncherHubWidget(QWidget* parent) : QWidget(parent)
{
	m_homeUrl = defaultHubUrl();

	auto* layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);

	m_tabsBarContainer = new QWidget(this);
	m_tabsBarContainer->setObjectName("hubTabsBar");
	auto* tabsLayout = new QHBoxLayout(m_tabsBarContainer);
	tabsLayout->setContentsMargins(10, 10, 10, 6);

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

	m_toolbarContainer = new QWidget(this);
	m_toolbarContainer->setObjectName("hubToolbar");
	auto* toolbar = new QHBoxLayout(m_toolbarContainer);
	toolbar->setContentsMargins(10, 8, 10, 10);

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
	m_homeButton->setToolTip(tr("Cockpit"));

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

#if defined(PROJT_USE_WEBENGINE)
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
	m_profile = sharedProfile;
#endif

	m_stack = new QStackedWidget(this);

	layout->addWidget(m_tabsBarContainer);
	layout->addWidget(m_toolbarContainer);
	layout->addWidget(m_stack);

	setStyleSheet(QStringLiteral(R"PROJT_HUB(
		LauncherHubWidget {
			background: #0f1420;
		}
		#hubTabsBar, #hubToolbar {
			background: #171f2e;
			border: 1px solid #2b3951;
			border-radius: 12px;
		}
		QTabBar::tab {
			background: transparent;
			color: #afbdd3;
			padding: 7px 13px;
			margin-right: 5px;
			border-radius: 9px;
		}
		QTabBar::tab:selected {
			background: #2f4f85;
			border: 1px solid #4a6ea9;
			color: #ffffff;
		}
		QTabBar::tab:hover {
			background: #253147;
			color: #ffffff;
		}
		QToolButton {
			background: #202b3f;
			border: 1px solid #334764;
			border-radius: 9px;
			padding: 6px;
		}
		QToolButton:hover {
			background: #2b3a54;
		}
		QLineEdit {
			background: #0d1422;
			color: #e6eefc;
			border: 1px solid #3a4c69;
			border-radius: 10px;
			padding: 8px 12px;
			selection-background-color: #42629a;
		}
		QScrollArea {
			border: none;
			background: transparent;
		}
		QFrame#hubHeroCard, QFrame#hubMetricCard, QFrame#hubPanel {
			background: #171f2e;
			border: 1px solid #2b3951;
			border-radius: 18px;
		}
		QLabel#hubBadge {
			background: #203454;
			border: 1px solid #33537f;
			border-radius: 10px;
			color: #cfe3ff;
			padding: 4px 10px;
			font-weight: 600;
		}
		QLabel#hubHeroTitle {
			color: #ffffff;
			font-size: 24px;
			font-weight: 700;
		}
		QLabel#hubHeroSubtitle, QLabel#hubPanelSubtitle, QLabel#hubMetricDetail {
			color: #9bb0cc;
		}
		QLabel#hubMetricValue {
			color: #ffffff;
			font-size: 22px;
			font-weight: 700;
		}
		QLabel#hubPanelTitle {
			color: #ffffff;
			font-size: 18px;
			font-weight: 700;
		}
		QPushButton#hubPrimaryButton, QPushButton#hubSecondaryButton, QPushButton#hubInlineAction,
		QPushButton#hubQuickButton, QPushButton#hubNewsButton {
			border-radius: 11px;
			padding: 10px 14px;
		}
		QPushButton#hubPrimaryButton {
			background: #4d7fff;
			border: 1px solid #6f9bff;
			color: #ffffff;
			font-weight: 700;
		}
		QPushButton#hubPrimaryButton:hover {
			background: #608cff;
		}
		QPushButton#hubSecondaryButton, QPushButton#hubInlineAction, QPushButton#hubQuickButton,
		QPushButton#hubNewsButton {
			background: #202b3f;
			border: 1px solid #334764;
			color: #e6eefc;
		}
		QPushButton#hubSecondaryButton:hover, QPushButton#hubInlineAction:hover, QPushButton#hubQuickButton:hover,
		QPushButton#hubNewsButton:hover {
			background: #2a3850;
		}
		QPushButton#hubQuickButton, QPushButton#hubNewsButton {
			text-align: left;
		}
		QPushButton#hubQuickButton[active="true"] {
			background: #233653;
			border-color: #5c82c4;
		}
	)PROJT_HUB"));

	connect(m_backButton,
			&QToolButton::clicked,
			this,
			[this]()
			{
				if (auto* view = currentView())
				{
					view->back();
				}
			});
	connect(m_forwardButton,
			&QToolButton::clicked,
			this,
			[this]()
			{
				if (auto* view = currentView())
				{
					view->forward();
				}
			});
	connect(m_reloadButton,
			&QToolButton::clicked,
			this,
			[this]()
			{
				if (auto* view = currentView())
				{
					view->reload();
				}
				else
				{
					refreshCockpit();
				}
			});
	connect(m_homeButton, &QToolButton::clicked, this, &LauncherHubWidget::loadHome);
	connect(m_goButton, &QToolButton::clicked, this, [this]() { openUrl(resolveInput(m_addressBar->text())); });
	connect(m_addressBar, &QLineEdit::returnPressed, this, [this]() { openUrl(resolveInput(m_addressBar->text())); });
	connect(m_newTabButton, &QToolButton::clicked, this, [this]() { newTab(m_homeUrl); });

	connect(m_tabBar,
			&QTabBar::tabMoved,
			this,
			[this](int from, int to)
			{
				if (!m_stack || from == to || from < 0 || to < 0 || from >= m_stack->count() || to >= m_stack->count())
				{
					return;
				}
				QWidget* page = m_stack->widget(from);
				if (!page)
				{
					return;
				}
				m_stack->removeWidget(page);
				m_stack->insertWidget(to, page);
				m_stack->setCurrentIndex(m_tabBar->currentIndex());
				updateTabPerformanceState();
				updateNavigationState();
			});
	connect(m_tabBar,
			&QTabBar::currentChanged,
			this,
			[this](int index)
			{
				if (index >= 0 && index < m_stack->count())
				{
					m_stack->setCurrentIndex(index);
					activatePendingForIndex(index);
					updateTabPerformanceState();
					updateNavigationState();
				}
			});
	connect(m_tabBar,
			&QTabBar::tabCloseRequested,
			this,
			[this](int index)
			{
				if (index < 0 || index >= m_stack->count())
				{
					return;
				}
				if (m_stack->widget(index) == m_cockpitPage)
				{
					return;
				}
				if (m_tabBar->count() == 1)
				{
					return;
				}

				QWidget* widget = m_stack->widget(index);
				m_stack->removeWidget(widget);
				m_tabBar->removeTab(index);
				widget->deleteLater();

				const int newIndex = qMin(index, m_tabBar->count() - 1);
				m_tabBar->setCurrentIndex(newIndex);
				m_stack->setCurrentIndex(newIndex);
				activatePendingForIndex(newIndex);
				updateTabPerformanceState();
				updateNavigationState();
			});

	m_newsChecker = new NewsChecker(APPLICATION->network(), BuildConfig.NEWS_RSS_URL);
	m_newsChecker->setParent(this);
	connect(m_newsChecker, &NewsChecker::newsLoaded, this, &LauncherHubWidget::rebuildNewsFeed);
	connect(m_newsChecker, &NewsChecker::newsLoadingFailed, this, &LauncherHubWidget::rebuildNewsFeed);

	if (APPLICATION->instances())
	{
		connect(APPLICATION->instances().get(),
				&InstanceList::instancesChanged,
				this,
				&LauncherHubWidget::refreshCockpit);
		connect(APPLICATION->instances().get(),
				&InstanceList::dataChanged,
				this,
				[this](const QModelIndex&, const QModelIndex&, const QList<int>&) { refreshCockpit(); });
	}
	if (APPLICATION->icons())
	{
		connect(APPLICATION->icons().get(),
				&projt::icons::IconList::iconUpdated,
				this,
				[this](const QString&) { refreshCockpit(); });
	}

	createCockpitTab();
	createTab(QUrl(BuildConfig.NEWS_OPEN_URL), tr("News"), false);
	if (!BuildConfig.HUB_COMMUNITY_URL.isEmpty())
	{
		createTab(QUrl(BuildConfig.HUB_COMMUNITY_URL), tr("Community"), false);
	}
	createTab(QUrl(BuildConfig.HELP_URL.arg("")), tr("Help"), false);

	refreshCockpit();
	m_newsChecker->reloadNews();
}

LauncherHubWidget::~LauncherHubWidget() = default;

HubView* LauncherHubWidget::currentView() const
{
	if (!m_stack)
	{
		return nullptr;
	}
	return qobject_cast<HubView*>(m_stack->currentWidget());
}

void LauncherHubWidget::createCockpitTab()
{
	auto* scrollArea = new QScrollArea(m_stack);
	scrollArea->setWidgetResizable(true);
	scrollArea->setFrameShape(QFrame::NoFrame);

	auto* content	 = new QWidget(scrollArea);
	auto* pageLayout = new QVBoxLayout(content);
	pageLayout->setContentsMargins(18, 18, 18, 18);
	pageLayout->setSpacing(16);

	auto* heroCard = new QFrame(content);
	heroCard->setObjectName("hubHeroCard");
	auto* heroLayout = new QVBoxLayout(heroCard);
	heroLayout->setContentsMargins(20, 20, 20, 20);
	heroLayout->setSpacing(14);

	auto* heroTop = new QHBoxLayout();
	heroTop->setSpacing(14);
	m_cockpitIconLabel = new QLabel(heroCard);
	m_cockpitIconLabel->setFixedSize(52, 52);
	m_cockpitIconLabel->setAlignment(Qt::AlignCenter);

	auto* heroText = new QVBoxLayout();
	heroText->setSpacing(6);
	m_cockpitBadgeLabel = new QLabel(heroCard);
	m_cockpitBadgeLabel->setObjectName("hubBadge");
	m_cockpitBadgeLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
	m_cockpitTitleLabel = new QLabel(heroCard);
	m_cockpitTitleLabel->setObjectName("hubHeroTitle");
	m_cockpitTitleLabel->setWordWrap(true);
	m_cockpitSubtitleLabel = new QLabel(heroCard);
	m_cockpitSubtitleLabel->setObjectName("hubHeroSubtitle");
	m_cockpitSubtitleLabel->setWordWrap(true);
	heroText->addWidget(m_cockpitBadgeLabel, 0, Qt::AlignLeft);
	heroText->addWidget(m_cockpitTitleLabel);
	heroText->addWidget(m_cockpitSubtitleLabel);

	heroTop->addWidget(m_cockpitIconLabel, 0, Qt::AlignTop);
	heroTop->addLayout(heroText, 1);
	heroLayout->addLayout(heroTop);

	auto* heroActions = new QHBoxLayout();
	heroActions->setSpacing(10);
	m_playButton = new QPushButton(tr("Play"), heroCard);
	m_playButton->setObjectName("hubPrimaryButton");
	m_editButton = new QPushButton(tr("Edit"), heroCard);
	m_editButton->setObjectName("hubSecondaryButton");
	m_backupsButton = new QPushButton(tr("Backups"), heroCard);
	m_backupsButton->setObjectName("hubSecondaryButton");
	m_folderButton = new QPushButton(tr("Open Folder"), heroCard);
	m_folderButton->setObjectName("hubSecondaryButton");
	heroActions->addWidget(m_playButton);
	heroActions->addWidget(m_editButton);
	heroActions->addWidget(m_backupsButton);
	heroActions->addWidget(m_folderButton);
	heroActions->addStretch(1);
	heroLayout->addLayout(heroActions);
	pageLayout->addWidget(heroCard);

	auto* metricsLayout = new QGridLayout();
	metricsLayout->setHorizontalSpacing(12);
	metricsLayout->setVerticalSpacing(12);
	auto makeMetricCard = [content](const QString& title, QLabel*& valueLabel, QLabel*& detailLabel)
	{
		auto* card = new QFrame(content);
		card->setObjectName("hubMetricCard");
		auto* cardLayout = new QVBoxLayout(card);
		cardLayout->setContentsMargins(16, 16, 16, 16);
		cardLayout->setSpacing(6);

		auto* titleLabel = new QLabel(title, card);
		titleLabel->setObjectName("hubPanelSubtitle");
		valueLabel = new QLabel(card);
		valueLabel->setObjectName("hubMetricValue");
		detailLabel = new QLabel(card);
		detailLabel->setObjectName("hubMetricDetail");
		detailLabel->setWordWrap(true);

		cardLayout->addWidget(titleLabel);
		cardLayout->addWidget(valueLabel);
		cardLayout->addWidget(detailLabel);
		return card;
	};

	metricsLayout->addWidget(makeMetricCard(tr("Instances"), m_instancesValueLabel, m_instancesDetailLabel), 0, 0);
	metricsLayout->addWidget(makeMetricCard(tr("Total Playtime"), m_playtimeValueLabel, m_playtimeDetailLabel), 0, 1);
	metricsLayout->addWidget(makeMetricCard(tr("Needs Attention"), m_attentionValueLabel, m_attentionDetailLabel),
							 0,
							 2);
	pageLayout->addLayout(metricsLayout);

	auto* lowerGrid = new QGridLayout();
	lowerGrid->setHorizontalSpacing(12);
	lowerGrid->setVerticalSpacing(12);

	auto* recentPanel = new QFrame(content);
	recentPanel->setObjectName("hubPanel");
	auto* recentPanelLayout = new QVBoxLayout(recentPanel);
	recentPanelLayout->setContentsMargins(16, 16, 16, 16);
	recentPanelLayout->setSpacing(10);
	auto* recentTitle = new QLabel(tr("Continue Playing"), recentPanel);
	recentTitle->setObjectName("hubPanelTitle");
	auto* recentSubtitle = new QLabel(tr("Jump back into your most recent worlds or packs."), recentPanel);
	recentSubtitle->setObjectName("hubPanelSubtitle");
	recentSubtitle->setWordWrap(true);
	recentPanelLayout->addWidget(recentTitle);
	recentPanelLayout->addWidget(recentSubtitle);
	m_recentInstancesLayout = new QVBoxLayout();
	m_recentInstancesLayout->setSpacing(8);
	recentPanelLayout->addLayout(m_recentInstancesLayout);
	recentPanelLayout->addStretch(1);
	lowerGrid->addWidget(recentPanel, 0, 0);

	auto* newsPanel = new QFrame(content);
	newsPanel->setObjectName("hubPanel");
	auto* newsPanelLayout = new QVBoxLayout(newsPanel);
	newsPanelLayout->setContentsMargins(16, 16, 16, 16);
	newsPanelLayout->setSpacing(10);
	auto* newsTitle = new QLabel(tr("Community Pulse"), newsPanel);
	newsTitle->setObjectName("hubPanelTitle");
	auto* newsSubtitle = new QLabel(tr("Latest launcher news without leaving the cockpit."), newsPanel);
	newsSubtitle->setObjectName("hubPanelSubtitle");
	newsSubtitle->setWordWrap(true);
	newsPanelLayout->addWidget(newsTitle);
	newsPanelLayout->addWidget(newsSubtitle);
	m_newsLayout = new QVBoxLayout();
	m_newsLayout->setSpacing(8);
	newsPanelLayout->addLayout(m_newsLayout);
	newsPanelLayout->addStretch(1);
	lowerGrid->addWidget(newsPanel, 0, 1);

	auto* linksPanel = new QFrame(content);
	linksPanel->setObjectName("hubPanel");
	auto* linksLayout = new QVBoxLayout(linksPanel);
	linksLayout->setContentsMargins(16, 16, 16, 16);
	linksLayout->setSpacing(10);
	auto* linksTitle = new QLabel(tr("Quick Routes"), linksPanel);
	linksTitle->setObjectName("hubPanelTitle");
	auto* linksSubtitle = new QLabel(tr("Open the spaces you reach for most while you play."), linksPanel);
	linksSubtitle->setObjectName("hubPanelSubtitle");
	linksSubtitle->setWordWrap(true);
	linksLayout->addWidget(linksTitle);
	linksLayout->addWidget(linksSubtitle);

	auto addLinkButton = [this, linksPanel, linksLayout](const QString& label, const QUrl& url)
	{
		auto* button = new QPushButton(label, linksPanel);
		button->setObjectName("hubSecondaryButton");
		connect(button, &QPushButton::clicked, this, [this, url]() { openUrl(url); });
		linksLayout->addWidget(button);
	};

	addLinkButton(tr("Open website"), m_homeUrl);
	addLinkButton(tr("Read news"), QUrl(BuildConfig.NEWS_OPEN_URL));
	if (!BuildConfig.HUB_COMMUNITY_URL.isEmpty())
	{
		addLinkButton(tr("Open community"), QUrl(BuildConfig.HUB_COMMUNITY_URL));
	}
	addLinkButton(tr("Open help"), QUrl(BuildConfig.HELP_URL.arg("")));
	lowerGrid->addWidget(linksPanel, 1, 0, 1, 2);

	pageLayout->addLayout(lowerGrid);
	pageLayout->addStretch(1);

	scrollArea->setWidget(content);
	m_cockpitPage	= scrollArea;
	const int index = m_stack->addWidget(m_cockpitPage);
	m_tabBar->addTab(tr("Cockpit"));
	m_tabBar->setCurrentIndex(index);
	m_stack->setCurrentIndex(index);

	connect(m_playButton,
			&QPushButton::clicked,
			this,
			[this]()
			{
				const QString instanceId = activeInstanceId();
				if (!instanceId.isEmpty())
				{
					emit launchInstanceRequested(instanceId);
				}
			});
	connect(m_editButton,
			&QPushButton::clicked,
			this,
			[this]()
			{
				const QString instanceId = activeInstanceId();
				if (!instanceId.isEmpty())
				{
					emit editInstanceRequested(instanceId);
				}
			});
	connect(m_backupsButton,
			&QPushButton::clicked,
			this,
			[this]()
			{
				const QString instanceId = activeInstanceId();
				if (!instanceId.isEmpty())
				{
					emit backupsRequested(instanceId);
				}
			});
	connect(m_folderButton,
			&QPushButton::clicked,
			this,
			[this]()
			{
				const QString instanceId = activeInstanceId();
				if (!instanceId.isEmpty())
				{
					emit openInstanceFolderRequested(instanceId);
				}
			});
}

HubView* LauncherHubWidget::createTab(const QUrl& url, const QString& label, bool switchTo)
{
	if (!m_stack || !m_tabBar)
	{
		return nullptr;
	}

	auto* view = new HubView(m_stack);
#if defined(PROJT_USE_WEBENGINE)
	auto* page = new LauncherHubPage(m_profile, view);
	view->setPage(page);
	view->setAttribute(Qt::WA_OpaquePaintEvent, true);
	view->setStyleSheet(QStringLiteral("background: #121822;"));
	page->setBackgroundColor(QColor(QStringLiteral("#121822")));
	view->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
	view->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, false);
	view->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, false);
	view->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, false);
	view->settings()->setAttribute(QWebEngineSettings::HyperlinkAuditingEnabled, false);
	view->settings()->setAttribute(QWebEngineSettings::ScrollAnimatorEnabled, false);
	view->settings()->setAttribute(QWebEngineSettings::WebGLEnabled, false);
	view->settings()->setAttribute(QWebEngineSettings::Accelerated2dCanvasEnabled, false);

	auto* channel = new QWebChannel(view);
	auto* bridge  = new LauncherHubBridge(channel);
	channel->registerObject(QStringLiteral("launcher"), bridge);
	page->setWebChannel(channel);
#endif

	const int stackIndex	   = m_stack->addWidget(view);
	const QString initialLabel = label.isEmpty() ? tr("New Tab") : label;
	m_tabBar->addTab(initialLabel);

	auto updateTitle = [this, view](const QString& title)
	{
		const int index = m_stack->indexOf(view);
		if (index >= 0 && !title.isEmpty())
		{
			m_tabBar->setTabText(index, title);
		}
	};

#if defined(PROJT_USE_WEBENGINE)
	connect(view, &QWebEngineView::titleChanged, this, updateTitle);
	connect(view,
			&QWebEngineView::urlChanged,
			this,
			[this, view](const QUrl& urlChanged)
			{
				if (view == currentView())
				{
					m_addressBar->setText(urlChanged.toString());
					updateNavigationState();
				}
			});
	connect(view,
			&QWebEngineView::loadFinished,
			this,
			[this, view](bool)
			{
				if (view == currentView())
				{
					updateNavigationState();
				}
			});
#elif defined(PROJT_USE_WEBVIEW2)
	connect(view, &WebView2Widget::titleChanged, this, updateTitle);
	connect(view,
			&WebView2Widget::urlChanged,
			this,
			[this, view](const QUrl& urlChanged)
			{
				if (view == currentView())
				{
					m_addressBar->setText(urlChanged.toString());
					updateNavigationState();
				}
			});
	connect(view,
			&WebView2Widget::loadFinished,
			this,
			[this, view](bool)
			{
				if (view == currentView())
				{
					updateNavigationState();
				}
			});
	connect(view, &WebView2Widget::navigationStateChanged, this, &LauncherHubWidget::updateNavigationState);
#else
	Q_UNUSED(updateTitle);
#endif

	if (switchTo)
	{
		m_tabBar->setCurrentIndex(stackIndex);
		m_stack->setCurrentIndex(stackIndex);
	}

	if (url.isValid())
	{
		const bool shouldLoadNow = switchTo;
		if (shouldLoadNow)
		{
			view->setUrl(url);
		}
		else
		{
			view->setProperty("hubPendingUrl", url);
		}
	}

	updateTabPerformanceState();
	return view;
}

void LauncherHubWidget::switchToPage(QWidget* page)
{
	if (!m_stack || !m_tabBar || !page)
	{
		return;
	}
	const int index = m_stack->indexOf(page);
	if (index < 0)
	{
		return;
	}

	m_tabBar->setCurrentIndex(index);
	m_stack->setCurrentIndex(index);
	activatePendingForIndex(index);
	updateTabPerformanceState();
	updateNavigationState();
}

void LauncherHubWidget::activatePendingForIndex(int index)
{
	if (!m_stack || index < 0 || index >= m_stack->count())
	{
		return;
	}
	if (auto* view = qobject_cast<HubView*>(m_stack->widget(index)))
	{
		const QUrl pendingUrl = view->property("hubPendingUrl").toUrl();
		if (pendingUrl.isValid())
		{
			view->setProperty("hubPendingUrl", QUrl());
			view->setUrl(pendingUrl);
		}
	}
}

void LauncherHubWidget::updateNavigationState()
{
	auto* view = currentView();
	if (!view)
	{
		m_backButton->setEnabled(false);
		m_forwardButton->setEnabled(false);
		m_goButton->setEnabled(false);
		m_addressBar->clear();
		m_addressBar->setEnabled(false);
		m_addressBar->setPlaceholderText(tr("Launcher Hub Cockpit"));
		return;
	}

	m_goButton->setEnabled(true);
	m_addressBar->setEnabled(true);
	m_addressBar->setPlaceholderText(tr("Search or enter address"));
#if defined(PROJT_USE_WEBENGINE)
	m_backButton->setEnabled(view->history()->canGoBack());
	m_forwardButton->setEnabled(view->history()->canGoForward());
#elif defined(PROJT_USE_WEBVIEW2)
	m_backButton->setEnabled(view->canGoBack());
	m_forwardButton->setEnabled(view->canGoForward());
#else
	m_backButton->setEnabled(view->canGoBack());
	m_forwardButton->setEnabled(view->canGoForward());
#endif
	m_addressBar->setText(view->url().toString());
}

void LauncherHubWidget::updateTabPerformanceState()
{
#if defined(PROJT_USE_WEBENGINE)
	if (!m_stack)
	{
		return;
	}

	const int activeIndex = m_stack->currentIndex();
	for (int i = 0; i < m_stack->count(); ++i)
	{
		auto* view = qobject_cast<QWebEngineView*>(m_stack->widget(i));
		if (!view || !view->page())
		{
			continue;
		}
		view->page()->setLifecycleState(i == activeIndex ? QWebEnginePage::LifecycleState::Active
														 : QWebEnginePage::LifecycleState::Frozen);
	}
#endif
}

void LauncherHubWidget::ensureLoaded()
{
	loadHome();
	m_loaded = true;
}

void LauncherHubWidget::loadHome()
{
	refreshCockpit();
	switchToPage(m_cockpitPage);
}

void LauncherHubWidget::newTab(const QUrl& url)
{
	createTab(url.isValid() ? url : m_homeUrl, QString(), true);
	m_loaded = true;
}

void LauncherHubWidget::openUrl(const QUrl& url)
{
	if (!url.isValid())
	{
		return;
	}

	auto* view = currentView();
	if (!view)
	{
		createTab(url, QString(), true);
		updateTabPerformanceState();
		updateNavigationState();
		m_loaded = true;
		return;
	}

	view->setUrl(url);
	updateTabPerformanceState();
	m_loaded = true;
}

void LauncherHubWidget::setHomeUrl(const QUrl& url)
{
	m_homeUrl = url;
	m_loaded  = false;
}

QUrl LauncherHubWidget::homeUrl() const
{
	return m_homeUrl;
}

void LauncherHubWidget::setSelectedInstanceId(const QString& id)
{
	m_selectedInstanceId = id;
	refreshCockpit();
}

QString LauncherHubWidget::activeInstanceId() const
{
	if (!m_selectedInstanceId.isEmpty())
	{
		return m_selectedInstanceId;
	}

	if (APPLICATION->settings())
	{
		const QString selected = APPLICATION->settings()->get("SelectedInstance").toString();
		if (!selected.isEmpty())
		{
			return selected;
		}
	}

	const QList<InstancePtr> instances = sortedInstances();
	if (!instances.isEmpty())
	{
		return instances.first()->id();
	}
	return {};
}

void LauncherHubWidget::refreshCockpit()
{
	if (!m_cockpitPage)
	{
		return;
	}

	if (m_newsChecker && !m_newsChecker->isLoadingNews() && m_newsChecker->getNewsEntries().isEmpty()
		&& m_newsChecker->getLastLoadErrorMsg().isEmpty())
	{
		m_newsChecker->reloadNews();
	}

	const QList<InstancePtr> instances = sortedInstances();
	int managedCount				   = 0;
	int attentionCount				   = 0;
	for (const auto& instance : instances)
	{
		if (instance->isManagedPack())
		{
			managedCount++;
		}
		if (instance->hasUpdateAvailable() || instance->hasCrashed() || instance->hasVersionBroken())
		{
			attentionCount++;
		}
	}

	if (m_instancesValueLabel)
	{
		m_instancesValueLabel->setText(QString::number(instances.size()));
	}
	if (m_instancesDetailLabel)
	{
		m_instancesDetailLabel->setText(instances.isEmpty() ? tr("No instances yet")
															: tr("%1 managed pack(s) in rotation").arg(managedCount));
	}

	const int totalPlaytime = APPLICATION->instances() ? APPLICATION->instances()->getTotalPlayTime() : 0;
	if (m_playtimeValueLabel)
	{
		m_playtimeValueLabel->setText(
			totalPlaytime > 0 ? Time::prettifyDuration(totalPlaytime,
													   APPLICATION->settings()->get("ShowGameTimeWithoutDays").toBool())
							  : tr("0m"));
	}
	if (m_playtimeDetailLabel)
	{
		m_playtimeDetailLabel->setText(tr("Your full launcher history across every instance."));
	}
	if (m_attentionValueLabel)
	{
		m_attentionValueLabel->setText(QString::number(attentionCount));
	}
	if (m_attentionDetailLabel)
	{
		m_attentionDetailLabel->setText(attentionCount > 0 ? tr("Updates, crashes, or broken versions to review.")
														   : tr("Everything looks healthy right now."));
	}

	updateHero();
	rebuildRecentInstances();
	rebuildNewsFeed();
}

void LauncherHubWidget::updateHero()
{
	const QString instanceId = activeInstanceId();
	const InstancePtr instance =
		APPLICATION->instances() ? APPLICATION->instances()->getInstanceById(instanceId) : nullptr;

	if (!instance)
	{
		m_cockpitBadgeLabel->setText(tr("Cockpit"));
		m_cockpitTitleLabel->setText(tr("Launcher Hub is ready"));
		m_cockpitSubtitleLabel->setText(tr("Open news, community pages, and help from one place. Once you create or "
										   "select an instance, it will appear here."));
		m_cockpitIconLabel->setPixmap(QIcon::fromTheme("applications-games").pixmap(40, 40));
		m_playButton->setEnabled(false);
		m_editButton->setEnabled(false);
		m_backupsButton->setEnabled(false);
		m_folderButton->setEnabled(false);
		return;
	}

	m_cockpitBadgeLabel->setText(heroBadgeForInstance(instance));
	m_cockpitTitleLabel->setText(instance->name());

	QString subtitle			 = instance->getStatusbarDescription();
	const QString lastLaunchText = relativeTimeLabel(instance->lastLaunch());
	if (!subtitle.isEmpty())
	{
		subtitle += tr("  |  Last launch: %1").arg(lastLaunchText);
	}
	else
	{
		subtitle = tr("Last launch: %1").arg(lastLaunchText);
	}
	m_cockpitSubtitleLabel->setText(subtitle);
	m_cockpitIconLabel->setPixmap(APPLICATION->icons()->getIcon(instance->iconKey()).pixmap(40, 40));
	m_playButton->setEnabled(instance->canLaunch() && !instance->isRunning());
	m_editButton->setEnabled(instance->canEdit());
	m_backupsButton->setEnabled(true);
	m_folderButton->setEnabled(true);
}

void LauncherHubWidget::rebuildRecentInstances()
{
	clearLayout(m_recentInstancesLayout);
	if (!m_recentInstancesLayout)
	{
		return;
	}

	const QList<InstancePtr> instances = sortedInstances();
	if (instances.isEmpty())
	{
		auto* label =
			new QLabel(tr("No instances yet. Your recent worlds and packs will show up here."), m_cockpitPage);
		label->setObjectName("hubPanelSubtitle");
		label->setWordWrap(true);
		m_recentInstancesLayout->addWidget(label);
		return;
	}

	const QString currentId = activeInstanceId();
	const int limit			= qMin(6, instances.size());
	for (int i = 0; i < limit; ++i)
	{
		const auto& instance = instances.at(i);
		auto* row			 = new QWidget(m_cockpitPage);
		auto* rowLayout		 = new QHBoxLayout(row);
		rowLayout->setContentsMargins(0, 0, 0, 0);
		rowLayout->setSpacing(8);

		auto* button =
			new QPushButton(QStringLiteral("%1\n%2").arg(
								instance->name(),
								tr("%1  |  %2").arg(instance->typeName(), relativeTimeLabel(instance->lastLaunch()))),
							row);
		button->setObjectName("hubQuickButton");
		button->setProperty("active", instance->id() == currentId);
		button->setIcon(APPLICATION->icons()->getIcon(instance->iconKey()));
		button->setIconSize(QSize(28, 28));
		button->setMinimumHeight(56);
		connect(button,
				&QPushButton::clicked,
				this,
				[this, instance]()
				{
					m_selectedInstanceId = instance->id();
					emit selectInstanceRequested(instance->id());
					refreshCockpit();
				});

		auto* launchButton = new QPushButton(tr("Play"), row);
		launchButton->setObjectName("hubInlineAction");
		launchButton->setEnabled(instance->canLaunch() && !instance->isRunning());
		connect(launchButton,
				&QPushButton::clicked,
				this,
				[this, instance]()
				{
					m_selectedInstanceId = instance->id();
					emit launchInstanceRequested(instance->id());
					refreshCockpit();
				});

		rowLayout->addWidget(button, 1);
		rowLayout->addWidget(launchButton);
		m_recentInstancesLayout->addWidget(row);
	}
}

void LauncherHubWidget::rebuildNewsFeed()
{
	clearLayout(m_newsLayout);
	if (!m_newsLayout || !m_newsChecker)
	{
		return;
	}

	const QList<NewsEntryPtr> entries = m_newsChecker->getNewsEntries();
	if (entries.isEmpty())
	{
		auto* label = new QLabel(m_newsChecker->isLoadingNews()
									 ? tr("Loading the latest posts...")
									 : tr("News is quiet right now. Use the button below to open the full feed."),
								 m_cockpitPage);
		label->setObjectName("hubPanelSubtitle");
		label->setWordWrap(true);
		m_newsLayout->addWidget(label);
	}
	else
	{
		const int limit = qMin(3, entries.size());
		for (int i = 0; i < limit; ++i)
		{
			const auto& entry = entries.at(i);
			auto* button = new QPushButton(QStringLiteral("%1\n%2").arg(entry->title, stripHtmlExcerpt(entry->content)),
										   m_cockpitPage);
			button->setObjectName("hubNewsButton");
			button->setMinimumHeight(66);
			connect(button,
					&QPushButton::clicked,
					this,
					[this, entry]()
					{ openUrl(QUrl(entry->link.isEmpty() ? BuildConfig.NEWS_OPEN_URL : entry->link)); });
			m_newsLayout->addWidget(button);
		}
	}

	auto* openFeedButton = new QPushButton(tr("Open full news feed"), m_cockpitPage);
	openFeedButton->setObjectName("hubInlineAction");
	connect(openFeedButton, &QPushButton::clicked, this, [this]() { openUrl(QUrl(BuildConfig.NEWS_OPEN_URL)); });
	m_newsLayout->addWidget(openFeedButton, 0, Qt::AlignLeft);
}

#endif // PROJT_DISABLE_LAUNCHER_HUB

#include "LauncherHubWidget.moc"
