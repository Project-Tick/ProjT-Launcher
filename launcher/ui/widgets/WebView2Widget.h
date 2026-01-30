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

class WebView2Widget : public QWidget
{
	Q_OBJECT

  public:
	explicit WebView2Widget(QWidget* parent = nullptr);
	~WebView2Widget() override;

	void setUrl(const QUrl& url);
	QUrl url() const;

	bool canGoBack() const;
	bool canGoForward() const;

  public slots:
	void back();
	void forward();
	void reload();

  signals:
	void titleChanged(const QString& title);
	void urlChanged(const QUrl& url);
	void loadFinished(bool ok);
	void navigationStateChanged();

  protected:
	void resizeEvent(QResizeEvent* event) override;

  private:
	void initialize();
	void updateBounds();
	void updateNavigationState();

	QUrl m_url;
	bool m_initialized = false;
	bool m_canGoBack  = false;
	bool m_canGoForward = false;

#if defined(PROJT_USE_WEBVIEW2) && defined(_WIN32)
	struct Impl;
	Impl* m_impl = nullptr;
#endif
};
