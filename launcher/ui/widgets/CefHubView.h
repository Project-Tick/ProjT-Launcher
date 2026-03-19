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

#include <QImage>
#include <QRect>
#include <QVector>

#if defined(PROJT_USE_CEF)

#include "include/cef_frame.h"

class QEvent;
class QChangeEvent;
class QFocusEvent;
class QHideEvent;
class QKeyEvent;
class QMouseEvent;
class QPaintEvent;
class QResizeEvent;
class QShowEvent;
class QWheelEvent;

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
	void setActive(bool active) override;

  public slots:
	void back() override;
	void forward() override;
	void reload() override;

  protected:
	void paintEvent(QPaintEvent* event) override;
	void changeEvent(QEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
	void showEvent(QShowEvent* event) override;
	void hideEvent(QHideEvent* event) override;
	void focusInEvent(QFocusEvent* event) override;
	void focusOutEvent(QFocusEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void wheelEvent(QWheelEvent* event) override;
	void keyPressEvent(QKeyEvent* event) override;
	void keyReleaseEvent(QKeyEvent* event) override;
	void leaveEvent(QEvent* event) override;

  public:
	void ensureBrowser();
	void syncNavigationState();
	void handleAddressChange(const QUrl& url);
	void handleTitleChange(const QString& title);
	void handleLoadingState(bool isLoading, bool canGoBack, bool canGoForward);
	void handleLoadFinished(bool ok);
	void handleBrowserClosed();
	void handlePopupRequest(const QUrl& url);
	void handlePopupVisibility(bool visible);
	void handlePopupRect(const QRect& rect);
	void handlePaint(bool popup, const QImage& image, const QVector<QRect>& dirtyRects);
	void applyLauncherTheme();
	void applyLauncherThemeToFrame(CefRefPtr<CefFrame> frame);

  private:
	QUrl m_url;
	QString m_title;
	bool m_canGoBack	= false;
	bool m_canGoForward = false;
	bool m_created		= false;
	bool m_closing		= false;
	bool m_active		= true;
	QImage m_viewImage;
	QImage m_popupImage;
	QRect m_popupRect;
	bool m_popupVisible = false;

	struct Impl;
	Impl* m_impl = nullptr;
};

#endif
