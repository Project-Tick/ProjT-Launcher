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

#include "WebView2Widget.h"

#include <QDir>
#include <QResizeEvent>
#include <QShowEvent>
#include <QStandardPaths>
#include <QTimer>

#if defined(PROJT_USE_WEBVIEW2) && defined(_WIN32)
#include <windows.h>
#include <wrl.h>
#include <WebView2.h>
#if __has_include(<WebView2Loader.h>)
#include <WebView2Loader.h>
#define PROJT_HAVE_WEBVIEW2_LOADER 1
#else
#define PROJT_HAVE_WEBVIEW2_LOADER 0
extern "C" HRESULT STDAPICALLTYPE CreateCoreWebView2EnvironmentWithOptions(
	PCWSTR browserExecutableFolder,
	PCWSTR userDataFolder,
	ICoreWebView2EnvironmentOptions* environmentOptions,
	ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler* environmentCreatedHandler);
#endif

using Microsoft::WRL::Callback;
using Microsoft::WRL::ComPtr;

struct WebView2Widget::Impl
{
	ComPtr<ICoreWebView2Environment> env;
	ComPtr<ICoreWebView2Controller> controller;
	ComPtr<ICoreWebView2> webview;
};
#endif

WebView2Widget::WebView2Widget(QWidget* parent) : HubViewBase(parent)
{
	setAttribute(Qt::WA_NativeWindow);
	setAttribute(Qt::WA_NoSystemBackground);

#if defined(PROJT_USE_WEBVIEW2) && defined(_WIN32)
	m_impl = new Impl();
	QTimer::singleShot(0, this, &WebView2Widget::initialize);
#endif
}

WebView2Widget::~WebView2Widget()
{
#if defined(PROJT_USE_WEBVIEW2) && defined(_WIN32)
	delete m_impl;
#endif
}

void WebView2Widget::setUrl(const QUrl& url)
{
	m_url = url;
#if defined(PROJT_USE_WEBVIEW2) && defined(_WIN32)
	if (m_impl && m_impl->webview)
	{
		const auto wide = url.toString().toStdWString();
		m_impl->webview->Navigate(wide.c_str());
		return;
	}
#endif
}

QUrl WebView2Widget::url() const
{
	return m_url;
}

bool WebView2Widget::canGoBack() const
{
	return m_canGoBack;
}

bool WebView2Widget::canGoForward() const
{
	return m_canGoForward;
}

void WebView2Widget::back()
{
#if defined(PROJT_USE_WEBVIEW2) && defined(_WIN32)
	if (m_impl && m_impl->webview && m_canGoBack)
	{
		m_impl->webview->GoBack();
	}
#endif
}

void WebView2Widget::forward()
{
#if defined(PROJT_USE_WEBVIEW2) && defined(_WIN32)
	if (m_impl && m_impl->webview && m_canGoForward)
	{
		m_impl->webview->GoForward();
	}
#endif
}

void WebView2Widget::reload()
{
#if defined(PROJT_USE_WEBVIEW2) && defined(_WIN32)
	if (m_impl && m_impl->webview)
	{
		m_impl->webview->Reload();
	}
#endif
}

void WebView2Widget::resizeEvent(QResizeEvent* event)
{
	QWidget::resizeEvent(event);
	updateBounds();
}

void WebView2Widget::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);
	updateBounds();
#if defined(PROJT_USE_WEBVIEW2) && defined(_WIN32)
	if (m_impl && m_impl->webview && m_url.isValid())
	{
		const auto wide = m_url.toString().toStdWString();
		m_impl->webview->Navigate(wide.c_str());
	}
#endif
}

void WebView2Widget::initialize()
{
#if defined(PROJT_USE_WEBVIEW2) && defined(_WIN32)
	if (m_initialized || !m_impl)
		return;

	m_initialized = true;
	const QString dataPath =
		QDir::cleanPath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/webview2");
	QDir().mkpath(dataPath);

	const auto dataPathWide = dataPath.toStdWString();
	const auto hwnd			= reinterpret_cast<HWND>(winId());
	HRESULT comInitResult	= CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	Q_UNUSED(comInitResult);

	CreateCoreWebView2EnvironmentWithOptions(
		nullptr,
		dataPathWide.c_str(),
		nullptr,
		Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
			[this, hwnd](HRESULT result, ICoreWebView2Environment* env) -> HRESULT
			{
				if (FAILED(result) || !env)
				{
					emit loadFinished(false);
					return S_OK;
				}

				m_impl->env = env;
				env->CreateCoreWebView2Controller(
					hwnd,
					Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
						[this](HRESULT controllerResult, ICoreWebView2Controller* controller) -> HRESULT
						{
							if (FAILED(controllerResult) || !controller)
							{
								emit loadFinished(false);
								return S_OK;
							}

							m_impl->controller = controller;
							m_impl->controller->get_CoreWebView2(&m_impl->webview);

							if (!m_impl->webview)
							{
								emit loadFinished(false);
								return S_OK;
							}

							updateBounds();

							EventRegistrationToken token{};
							m_impl->webview->add_DocumentTitleChanged(
								Callback<ICoreWebView2DocumentTitleChangedEventHandler>(
									[this](ICoreWebView2*, IUnknown*) -> HRESULT
									{
										LPWSTR title = nullptr;
										if (SUCCEEDED(m_impl->webview->get_DocumentTitle(&title)) && title)
										{
											emit titleChanged(QString::fromWCharArray(title));
											CoTaskMemFree(title);
										}
										return S_OK;
									})
									.Get(),
								&token);

							m_impl->webview->add_SourceChanged(
								Callback<ICoreWebView2SourceChangedEventHandler>(
									[this](ICoreWebView2*, ICoreWebView2SourceChangedEventArgs*) -> HRESULT
									{
										LPWSTR uri = nullptr;
										if (SUCCEEDED(m_impl->webview->get_Source(&uri)) && uri)
										{
											m_url = QUrl(QString::fromWCharArray(uri));
											emit urlChanged(m_url);
											CoTaskMemFree(uri);
										}
										updateNavigationState();
										return S_OK;
									})
									.Get(),
								&token);

							m_impl->webview->add_NavigationCompleted(
								Callback<ICoreWebView2NavigationCompletedEventHandler>(
									[this](ICoreWebView2*, ICoreWebView2NavigationCompletedEventArgs* args) -> HRESULT
									{
										BOOL success = FALSE;
										if (args)
										{
											args->get_IsSuccess(&success);
										}
										updateNavigationState();
										emit loadFinished(success == TRUE);
										return S_OK;
									})
									.Get(),
								&token);

							m_impl->webview->add_HistoryChanged(Callback<ICoreWebView2HistoryChangedEventHandler>(
																	[this](ICoreWebView2*, IUnknown*) -> HRESULT
																	{
																		updateNavigationState();
																		return S_OK;
																	})
																	.Get(),
																&token);

							if (m_url.isValid())
							{
								const auto wideUrl = m_url.toString().toStdWString();
								m_impl->webview->Navigate(wideUrl.c_str());
							}

							return S_OK;
						})
						.Get());

				return S_OK;
			})
			.Get());
#else
	emit loadFinished(false);
#endif
}

void WebView2Widget::updateBounds()
{
#if defined(PROJT_USE_WEBVIEW2) && defined(_WIN32)
	if (!m_impl || !m_impl->controller)
		return;

	RECT bounds{};
	bounds.left	  = 0;
	bounds.top	  = 0;
	bounds.right  = width();
	bounds.bottom = height();
	m_impl->controller->put_Bounds(bounds);
#endif
}

void WebView2Widget::updateNavigationState()
{
#if defined(PROJT_USE_WEBVIEW2) && defined(_WIN32)
	if (!m_impl || !m_impl->webview)
		return;

	BOOL canBack	= FALSE;
	BOOL canForward = FALSE;
	m_impl->webview->get_CanGoBack(&canBack);
	m_impl->webview->get_CanGoForward(&canForward);

	const bool newCanBack	 = (canBack == TRUE);
	const bool newCanForward = (canForward == TRUE);
	if (newCanBack != m_canGoBack || newCanForward != m_canGoForward)
	{
		m_canGoBack	   = newCanBack;
		m_canGoForward = newCanForward;
		emit navigationStateChanged();
	}
#endif
}
