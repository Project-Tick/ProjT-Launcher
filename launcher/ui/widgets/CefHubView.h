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

#include "ui/widgets/HubViewBase.h"

#if defined(PROJT_USE_CEF)

class QResizeEvent;
class QShowEvent;

class CefHubView : public HubViewBase
{
	Q_OBJECT

  public:
	explicit CefHubView(QWidget* parent = nullptr);
	~CefHubView() override;

	void setUrl(const QUrl& url) override;
	QUrl url() const override;
	bool canGoBack() const override;
	bool canGoForward() const override;

  public slots:
	void back() override;
	void forward() override;
	void reload() override;

  protected:
	void resizeEvent(QResizeEvent* event) override;
	void showEvent(QShowEvent* event) override;

  public:
	void ensureBrowser();
	void syncNavigationState();
	void handleAddressChange(const QUrl& url);
	void handleTitleChange(const QString& title);
	void handleLoadingState(bool isLoading, bool canGoBack, bool canGoForward);
	void handleLoadFinished(bool ok);
	void handleBrowserClosed();

  private:
	QUrl m_url;
	QString m_title;
	bool m_canGoBack	= false;
	bool m_canGoForward = false;
	bool m_created		= false;
	bool m_closing		= false;

	struct Impl;
	Impl* m_impl = nullptr;
};

#endif
