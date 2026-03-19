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
#include <QEvent>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QLocale>
#include <QPainter>
#include <QPushButton>
#include <QScrollArea>
#include <QSignalBlocker>
#include <QSizePolicy>
#include <QStackedWidget>
#include <QTabBar>
#include <QTextDocumentFragment>
#include <QToolButton>
#include <QVBoxLayout>

#include "Application.h"
#include "BaseInstance.h"
#include "BuildConfig.h"
#include "InstanceList.h"
#include "MMCTime.h"
#include "icons/IconList.hpp"
#include "news/NewsChecker.h"
#include "ui/widgets/CefHubView.h"
#include "ui/widgets/FallbackHubView.h"
#include "ui/widgets/HubSearchProvider.h"
#include "ui/widgets/WebView2Widget.h"
#include "ui/widgets/QtWebEngineHubView.h"

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

void LauncherHubWidget::changeEvent(QEvent* event)
{
	QWidget::changeEvent(event);
}

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
				delete widget;
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

	QIcon tintedIcon(const QString& themeName, const QWidget* widget, const QSize& size = QSize(18, 18))
	{
		QIcon source = QIcon::fromTheme(themeName);
		if (source.isNull())
		{
			return source;
		}

		const qreal devicePixelRatio = widget ? widget->devicePixelRatioF() : qApp->devicePixelRatio();
		const QSize pixelSize = QSize(qMax(1, qRound(size.width() * devicePixelRatio)),
									  qMax(1, qRound(size.height() * devicePixelRatio)));

		auto colorizedPixmap = [&](QIcon::Mode mode)
		{
			QPixmap sourcePixmap = source.pixmap(pixelSize, devicePixelRatio, mode);
			if (sourcePixmap.isNull())
			{
				sourcePixmap = source.pixmap(pixelSize, devicePixelRatio, QIcon::Normal);
			}

			QPixmap tinted(pixelSize);
			tinted.fill(Qt::transparent);

			const QColor color = widget ? widget->palette().color(mode == QIcon::Disabled ? QPalette::Disabled
																					  : QPalette::Active,
															 QPalette::ButtonText)
									 : QColor(Qt::white);

			QPainter painter(&tinted);
			painter.drawPixmap(0, 0, sourcePixmap);
			painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
			painter.fillRect(tinted.rect(), color);
			painter.end();
			tinted.setDevicePixelRatio(devicePixelRatio);
			return tinted;
		};

		QIcon icon;
		icon.addPixmap(colorizedPixmap(QIcon::Normal), QIcon::Normal);
		icon.addPixmap(colorizedPixmap(QIcon::Disabled), QIcon::Disabled);
		icon.addPixmap(colorizedPixmap(QIcon::Active), QIcon::Active);
		icon.addPixmap(colorizedPixmap(QIcon::Selected), QIcon::Selected);
		return icon;
	}
	HubViewBase* createBrowserView(QWidget* parent)
	{
#if defined(PROJT_USE_WEBVIEW2)
		return new WebView2Widget(parent);
#elif defined(PROJT_USE_WEBENGINE)
		return new QtWebEngineHubView(parent);
#elif defined(PROJT_USE_CEF)
		return new CefHubView(parent);
#else
		return new FallbackHubView(QObject::tr("This page opens in your browser on this platform."), parent);
#endif
	}
}

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
	tabsLayout->addWidget(m_tabBar, 1);

	m_toolbarContainer = new QWidget(this);
	m_toolbarContainer->setObjectName("hubToolbar");
	auto* toolbar = new QHBoxLayout(m_toolbarContainer);
	toolbar->setContentsMargins(10, 8, 10, 10);

	m_backButton = new QToolButton(this);
	m_backButton->setToolTip(tr("Back"));
	m_backButton->setEnabled(false);

	m_forwardButton = new QToolButton(this);
	m_forwardButton->setToolTip(tr("Forward"));
	m_forwardButton->setEnabled(false);

	m_reloadButton = new QToolButton(this);
	m_reloadButton->setToolTip(tr("Reload"));

	m_homeButton = new QToolButton(this);
	m_homeButton->setToolTip(tr("Cockpit"));

	m_newTabButton = new QToolButton(this);
	m_newTabButton->setToolTip(tr("New Tab"));

	m_addressBar = new QLineEdit(this);
	m_addressBar->setPlaceholderText(tr("Search or enter address"));
	m_addressBar->setClearButtonEnabled(true);

	m_goButton = new QToolButton(this);
	m_goButton->setToolTip(tr("Go"));

	toolbar->addWidget(m_backButton);
	toolbar->addWidget(m_forwardButton);
	toolbar->addWidget(m_reloadButton);
	toolbar->addWidget(m_homeButton);
	toolbar->addWidget(m_newTabButton);
	toolbar->addWidget(m_addressBar, 1);
	toolbar->addWidget(m_goButton);

	m_stack = new QStackedWidget(this);

	layout->addWidget(m_tabsBarContainer);
	layout->addWidget(m_toolbarContainer);
	layout->addWidget(m_stack);

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
	connect(m_goButton,
			&QToolButton::clicked,
			this,
			[this]()
			{
				const QString providerId =
					APPLICATION->settings() ? APPLICATION->settings()->get("HubSearchEngine").toString() : QString();
				openUrl(resolveHubInput(m_addressBar->text(), providerId));
			});
	connect(m_addressBar,
			&QLineEdit::returnPressed,
			this,
			[this]()
			{
				const QString providerId =
					APPLICATION->settings() ? APPLICATION->settings()->get("HubSearchEngine").toString() : QString();
				openUrl(resolveHubInput(m_addressBar->text(), providerId));
			});
	connect(m_newTabButton, &QToolButton::clicked, this, [this]() { newTab(m_homeUrl); });

	connect(m_tabBar,
			&QTabBar::tabMoved,
			this,
			[this](int, int)
			{
				updateTabPerformanceState();
				updateNavigationState();
			});
	connect(m_tabBar,
			&QTabBar::currentChanged,
			this,
			[this](int index)
			{
				if (auto* view = viewForTabIndex(index))
				{
					m_stack->setCurrentWidget(view);
					activatePendingForPage(view);
					updateTabPerformanceState();
					updateNavigationState();
				}
			});
	connect(m_tabBar,
			&QTabBar::tabCloseRequested,
			this,
			[this](int index)
			{
				auto* view = viewForTabIndex(index);
				if (!view)
				{
					return;
				}

				m_stack->removeWidget(view);
				m_tabBar->removeTab(index);
				view->deleteLater();

				if (m_tabBar->count() > 0)
				{
					const int newIndex = qMin(index, m_tabBar->count() - 1);
					m_tabBar->setCurrentIndex(newIndex);
					if (auto* nextView = viewForTabIndex(newIndex))
					{
						m_stack->setCurrentWidget(nextView);
						activatePendingForPage(nextView);
					}
				}
				else
				{
					switchToPage(m_cockpitPage);
				}

				syncTabsUi();
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
	refreshToolbarIcons();

	refreshCockpit();
	m_newsChecker->reloadNews();
	syncTabsUi();
}

LauncherHubWidget::~LauncherHubWidget() = default;

HubViewBase* LauncherHubWidget::currentView() const
{
	if (!m_stack)
	{
		return nullptr;
	}
	return qobject_cast<HubViewBase*>(m_stack->currentWidget());
}

HubViewBase* LauncherHubWidget::viewForTabIndex(int index) const
{
	if (!m_tabBar || index < 0 || index >= m_tabBar->count())
	{
		return nullptr;
	}

	return qobject_cast<HubViewBase*>(m_tabBar->tabData(index).value<QObject*>());
}

int LauncherHubWidget::tabIndexForView(const HubViewBase* view) const
{
	if (!m_tabBar || !view)
	{
		return -1;
	}

	for (int i = 0; i < m_tabBar->count(); ++i)
	{
		if (m_tabBar->tabData(i).value<QObject*>() == view)
		{
			return i;
		}
	}
	return -1;
}

void LauncherHubWidget::createCockpitTab()
{
	auto* scrollArea = new QScrollArea(m_stack);
	scrollArea->setWidgetResizable(true);
	scrollArea->setFrameShape(QFrame::NoFrame);
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	auto* content	 = new QWidget(scrollArea);
	content->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
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
	metricsLayout->setColumnStretch(0, 1);
	metricsLayout->setColumnStretch(1, 1);
	metricsLayout->setColumnStretch(2, 1);
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
	lowerGrid->setColumnStretch(0, 1);
	lowerGrid->setColumnStretch(1, 1);

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
	m_cockpitPage = scrollArea;
	m_stack->addWidget(m_cockpitPage);
	m_stack->setCurrentWidget(m_cockpitPage);

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

void LauncherHubWidget::changeEvent(QEvent* event)
{
	QWidget::changeEvent(event);
	if (!event)
	{
		return;
	}

	if (event->type() == QEvent::PaletteChange || event->type() == QEvent::ApplicationPaletteChange)
	{
		refreshToolbarIcons();
		updateHero();
	}
}

HubViewBase* LauncherHubWidget::createTab(const QUrl& url, const QString& label, bool switchTo)
{
	if (!m_stack || !m_tabBar)
	{
		return nullptr;
	}

	auto* view = createBrowserView(m_stack);

	QWidget* previousPage	   = m_stack->currentWidget();
	const int previousTabIndex = m_tabBar->currentIndex();

	m_stack->addWidget(view);
	const QString initialLabel = label.isEmpty() ? tr("New Tab") : label;
	int tabIndex			   = -1;
	if (switchTo)
	{
		tabIndex = m_tabBar->addTab(initialLabel);
	}
	else
	{
		const QSignalBlocker blocker(m_tabBar);
		tabIndex = m_tabBar->addTab(initialLabel);
		m_tabBar->setCurrentIndex(previousTabIndex);
	}
	m_tabBar->setTabData(tabIndex, QVariant::fromValue(static_cast<QObject*>(view)));

	auto updateTitle = [this, view](const QString& title)
	{
		const int index = tabIndexForView(view);
		if (index >= 0 && !title.isEmpty())
		{
			m_tabBar->setTabText(index, title);
		}
	};

	connect(view, &HubViewBase::titleChanged, this, updateTitle);
	connect(view,
			&HubViewBase::urlChanged,
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
			&HubViewBase::loadFinished,
			this,
			[this, view](bool)
			{
				if (view == currentView())
				{
					updateNavigationState();
				}
			});
	connect(view, &HubViewBase::navigationStateChanged, this, &LauncherHubWidget::updateNavigationState);
	connect(view,
			&HubViewBase::newTabRequested,
			this,
			[this](const QUrl& requestedUrl)
			{
				if (!requestedUrl.isValid())
				{
					return;
				}

				createTab(requestedUrl, QString(), true);
			});

	if (switchTo)
	{
		m_tabBar->setCurrentIndex(tabIndex);
		m_stack->setCurrentWidget(view);
	}
	else if (previousPage)
	{
		m_stack->setCurrentWidget(previousPage);
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

	syncTabsUi();
	updateTabPerformanceState();
	return view;
}

void LauncherHubWidget::switchToPage(QWidget* page)
{
	if (!m_stack || !m_tabBar || !page)
	{
		return;
	}

	m_stack->setCurrentWidget(page);
	if (page == m_cockpitPage)
	{
		updateTabPerformanceState();
		updateNavigationState();
		syncTabsUi();
		return;
	}

	if (auto* view = qobject_cast<HubViewBase*>(page))
	{
		const int index = tabIndexForView(view);
		if (index >= 0)
		{
			m_tabBar->setCurrentIndex(index);
		}
		activatePendingForPage(view);
	}

	updateTabPerformanceState();
	updateNavigationState();
	syncTabsUi();
}

void LauncherHubWidget::activatePendingForPage(QWidget* page)
{
	if (!page)
	{
		return;
	}
	if (auto* view = qobject_cast<HubViewBase*>(page))
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
	m_backButton->setEnabled(view->canGoBack());
	m_forwardButton->setEnabled(view->canGoForward());
	m_addressBar->setText(view->url().toString());
}

void LauncherHubWidget::syncTabsUi()
{
	if (m_tabsBarContainer && m_tabBar)
	{
		m_tabsBarContainer->setVisible(m_tabBar->count() > 0);
	}
}

void LauncherHubWidget::refreshToolbarIcons()
{
	if (m_backButton)
	{
		m_backButton->setIcon(tintedIcon(QStringLiteral("go-previous"), this));
	}
	if (m_forwardButton)
	{
		m_forwardButton->setIcon(tintedIcon(QStringLiteral("go-next"), this));
	}
	if (m_reloadButton)
	{
		m_reloadButton->setIcon(tintedIcon(QStringLiteral("view-refresh"), this));
	}
	if (m_homeButton)
	{
		m_homeButton->setIcon(tintedIcon(QStringLiteral("go-home"), this));
	}
	if (m_newTabButton)
	{
		m_newTabButton->setIcon(tintedIcon(QStringLiteral("list-add"), this));
	}
	if (m_goButton)
	{
		m_goButton->setIcon(tintedIcon(QStringLiteral("system-search"), this));
	}
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
		auto* view = qobject_cast<HubViewBase*>(m_stack->widget(i));
		if (!view)
		{
			continue;
		}
		view->setActive(i == activeIndex);
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
		m_cockpitIconLabel->setPixmap(APPLICATION->logo().pixmap(40, 40));
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
