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

#include "CefHubView.h"

#if defined(PROJT_USE_CEF)

#include <QMetaObject>
#include <QResizeEvent>
#include <QShowEvent>
#include <QTimer>

#include "CefRuntime.h"

#include "include/cef_browser.h"
#include "include/cef_client.h"
#include "include/cef_display_handler.h"
#include "include/cef_life_span_handler.h"
#include "include/cef_load_handler.h"
#include "include/wrapper/cef_helpers.h"

namespace
{
	class CefHubClient final : public CefClient,
							   public CefDisplayHandler,
							   public CefLifeSpanHandler,
							   public CefLoadHandler
	{
	  public:
		explicit CefHubClient(CefHubView* owner) : m_owner(owner)
		{}

		CefRefPtr<CefDisplayHandler> GetDisplayHandler() override
		{
			return this;
		}

		CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override
		{
			return this;
		}

		CefRefPtr<CefLoadHandler> GetLoadHandler() override
		{
			return this;
		}

		void OnAddressChange(CefRefPtr<CefBrowser>, CefRefPtr<CefFrame> frame, const CefString& url) override
		{
			if (!m_owner || !frame || !frame->IsMain())
			{
				return;
			}
			const QUrl qtUrl(QString::fromStdString(url.ToString()));
			QMetaObject::invokeMethod(
				m_owner, [owner = m_owner, qtUrl]() { owner->handleAddressChange(qtUrl); }, Qt::QueuedConnection);
		}

		void OnTitleChange(CefRefPtr<CefBrowser>, const CefString& title) override
		{
			const QString qtTitle = QString::fromStdString(title.ToString());
			QMetaObject::invokeMethod(
				m_owner, [owner = m_owner, qtTitle]() { owner->handleTitleChange(qtTitle); }, Qt::QueuedConnection);
		}

		void OnLoadingStateChange(CefRefPtr<CefBrowser>, bool isLoading, bool canGoBack, bool canGoForward) override
		{
			QMetaObject::invokeMethod(
				m_owner,
				[owner = m_owner, isLoading, canGoBack, canGoForward]()
				{ owner->handleLoadingState(isLoading, canGoBack, canGoForward); },
				Qt::QueuedConnection);
		}

		void OnLoadEnd(CefRefPtr<CefBrowser>, CefRefPtr<CefFrame> frame, int) override
		{
			if (!m_owner || !frame || !frame->IsMain())
			{
				return;
			}
			QMetaObject::invokeMethod(
				m_owner, [owner = m_owner]() { owner->handleLoadFinished(true); }, Qt::QueuedConnection);
		}

		void OnLoadError(CefRefPtr<CefBrowser>, CefRefPtr<CefFrame> frame, ErrorCode, const CefString&, const CefString&) override
		{
			if (!m_owner || !frame || !frame->IsMain())
			{
				return;
			}
			QMetaObject::invokeMethod(
				m_owner, [owner = m_owner]() { owner->handleLoadFinished(false); }, Qt::QueuedConnection);
		}

		void OnAfterCreated(CefRefPtr<CefBrowser> browser) override
		{
			m_browser = browser;
		}

		void OnBeforeClose(CefRefPtr<CefBrowser>) override
		{
			QMetaObject::invokeMethod(
				m_owner, [owner = m_owner]() { owner->handleBrowserClosed(); }, Qt::QueuedConnection);
			m_browser = nullptr;
		}

		CefRefPtr<CefBrowser> browser() const
		{
			return m_browser;
		}

		IMPLEMENT_REFCOUNTING(CefHubClient);

	  private:
		CefHubView* m_owner = nullptr;
		CefRefPtr<CefBrowser> m_browser;
	};
}

struct CefHubView::Impl
{
	CefRefPtr<CefHubClient> client;
};

CefHubView::CefHubView(QWidget* parent) : HubViewBase(parent), m_impl(new Impl())
{
	setAttribute(Qt::WA_NativeWindow);
	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_OpaquePaintEvent);
}

CefHubView::~CefHubView()
{
	if (m_impl && m_impl->client && m_impl->client->browser() && !m_closing)
	{
		m_closing = true;
		m_impl->client->browser()->GetHost()->CloseBrowser(true);
	}
	delete m_impl;
}

void CefHubView::setUrl(const QUrl& url)
{
	m_url = url;
	ensureBrowser();

	if (m_impl && m_impl->client && m_impl->client->browser() && m_url.isValid())
	{
		m_impl->client->browser()->GetMainFrame()->LoadURL(m_url.toString().toStdString());
	}
}

QUrl CefHubView::url() const
{
	return m_url;
}

bool CefHubView::canGoBack() const
{
	return m_canGoBack;
}

bool CefHubView::canGoForward() const
{
	return m_canGoForward;
}

void CefHubView::back()
{
	if (m_impl && m_impl->client && m_impl->client->browser() && m_canGoBack)
	{
		m_impl->client->browser()->GoBack();
	}
}

void CefHubView::forward()
{
	if (m_impl && m_impl->client && m_impl->client->browser() && m_canGoForward)
	{
		m_impl->client->browser()->GoForward();
	}
}

void CefHubView::reload()
{
	if (m_impl && m_impl->client && m_impl->client->browser())
	{
		m_impl->client->browser()->Reload();
	}
}

void CefHubView::resizeEvent(QResizeEvent* event)
{
	HubViewBase::resizeEvent(event);
	if (m_impl && m_impl->client && m_impl->client->browser())
	{
		m_impl->client->browser()->GetHost()->NotifyMoveOrResizeStarted();
		m_impl->client->browser()->GetHost()->WasResized();
	}
	else
	{
		QTimer::singleShot(0, this, &CefHubView::ensureBrowser);
	}
}

void CefHubView::showEvent(QShowEvent* event)
{
	HubViewBase::showEvent(event);
	QTimer::singleShot(0, this, &CefHubView::ensureBrowser);
}

void CefHubView::ensureBrowser()
{
	if (m_created || width() <= 0 || height() <= 0 || !projt::cef::Runtime::instance().isInitialized())
	{
		return;
	}

	m_impl->client = new CefHubClient(this);

	CefWindowInfo windowInfo;
	windowInfo.SetAsChild(static_cast<CefWindowHandle>(winId()), CefRect(0, 0, width(), height()));

	CefBrowserSettings browserSettings;
	const QString initialUrl = m_url.isValid() ? m_url.toString() : QStringLiteral("about:blank");
	auto browser = CefBrowserHost::CreateBrowserSync(windowInfo,
													 m_impl->client,
													 initialUrl.toStdString(),
													 browserSettings,
													 nullptr,
													 nullptr);
	if (!browser)
	{
		emit loadFinished(false);
		return;
	}

	m_created = true;
	syncNavigationState();
}

void CefHubView::syncNavigationState()
{
	if (!m_impl || !m_impl->client || !m_impl->client->browser())
	{
		return;
	}

	const bool oldCanGoBack	 = m_canGoBack;
	const bool oldCanGoForward = m_canGoForward;
	m_canGoBack				 = m_impl->client->browser()->CanGoBack();
	m_canGoForward			 = m_impl->client->browser()->CanGoForward();
	if (oldCanGoBack != m_canGoBack || oldCanGoForward != m_canGoForward)
	{
		emit navigationStateChanged();
	}
}

void CefHubView::handleAddressChange(const QUrl& url)
{
	m_url = url;
	emit urlChanged(m_url);
	syncNavigationState();
}

void CefHubView::handleTitleChange(const QString& title)
{
	m_title = title;
	emit titleChanged(m_title);
}

void CefHubView::handleLoadingState(bool isLoading, bool canGoBack, bool canGoForward)
{
	Q_UNUSED(isLoading);
	m_canGoBack	 = canGoBack;
	m_canGoForward = canGoForward;
	emit navigationStateChanged();
}

void CefHubView::handleLoadFinished(bool ok)
{
	syncNavigationState();
	emit loadFinished(ok);
}

void CefHubView::handleBrowserClosed()
{
	m_created = false;
	m_closing = true;
	emit navigationStateChanged();
}

#endif
