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

#include <algorithm>
#include <cmath>
#include <QApplication>
#include <QColor>
#include <QFocusEvent>
#include <QHideEvent>
#include <QKeyEvent>
#include <QMetaObject>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QScreen>
#include <QShowEvent>
#include <QTimer>
#include <QWheelEvent>

#include "CefRuntime.h"

#include "include/cef_browser.h"
#include "include/cef_client.h"
#include "include/cef_dialog_handler.h"
#include "include/cef_display_handler.h"
#include "include/cef_jsdialog_handler.h"
#include "include/cef_life_span_handler.h"
#include "include/cef_load_handler.h"
#include "include/cef_permission_handler.h"
#include "include/cef_request_context.h"
#include "include/cef_render_handler.h"
#include "include/cef_request_handler.h"
#include "include/internal/cef_types_wrappers.h"
#include "include/wrapper/cef_helpers.h"

namespace
{
	uint32_t toCefModifiers(Qt::KeyboardModifiers keyboardModifiers, Qt::MouseButtons mouseButtons)
	{
		uint32_t modifiers = EVENTFLAG_NONE;
		if (keyboardModifiers.testFlag(Qt::ShiftModifier))
		{
			modifiers |= EVENTFLAG_SHIFT_DOWN;
		}
		if (keyboardModifiers.testFlag(Qt::ControlModifier))
		{
			modifiers |= EVENTFLAG_CONTROL_DOWN;
		}
		if (keyboardModifiers.testFlag(Qt::AltModifier))
		{
			modifiers |= EVENTFLAG_ALT_DOWN;
		}
		if (keyboardModifiers.testFlag(Qt::MetaModifier))
		{
			modifiers |= EVENTFLAG_COMMAND_DOWN;
		}
		if (mouseButtons.testFlag(Qt::LeftButton))
		{
			modifiers |= EVENTFLAG_LEFT_MOUSE_BUTTON;
		}
		if (mouseButtons.testFlag(Qt::MiddleButton))
		{
			modifiers |= EVENTFLAG_MIDDLE_MOUSE_BUTTON;
		}
		if (mouseButtons.testFlag(Qt::RightButton))
		{
			modifiers |= EVENTFLAG_RIGHT_MOUSE_BUTTON;
		}
		return modifiers;
	}

	CefMouseEvent toCefMouseEvent(const QPointF& localPosition,
								  Qt::KeyboardModifiers keyboardModifiers,
								  Qt::MouseButtons mouseButtons)
	{
		CefMouseEvent event;
		event.x			= static_cast<int>(std::lround(localPosition.x()));
		event.y			= static_cast<int>(std::lround(localPosition.y()));
		event.modifiers = toCefModifiers(keyboardModifiers, mouseButtons);
		return event;
	}

	cef_mouse_button_type_t toCefMouseButton(Qt::MouseButton button)
	{
		switch (button)
		{
			case Qt::LeftButton: return MBT_LEFT;
			case Qt::MiddleButton: return MBT_MIDDLE;
			case Qt::RightButton: return MBT_RIGHT;
			default: return MBT_LEFT;
		}
	}

	int toWindowsKeyCode(const QKeyEvent* event)
	{
		switch (event->key())
		{
			case Qt::Key_Backspace: return 0x08;
			case Qt::Key_Tab: return 0x09;
			case Qt::Key_Return:
			case Qt::Key_Enter: return 0x0D;
			case Qt::Key_Shift: return 0x10;
			case Qt::Key_Control: return 0x11;
			case Qt::Key_Alt: return 0x12;
			case Qt::Key_Pause: return 0x13;
			case Qt::Key_CapsLock: return 0x14;
			case Qt::Key_Escape: return 0x1B;
			case Qt::Key_Space: return 0x20;
			case Qt::Key_PageUp: return 0x21;
			case Qt::Key_PageDown: return 0x22;
			case Qt::Key_End: return 0x23;
			case Qt::Key_Home: return 0x24;
			case Qt::Key_Left: return 0x25;
			case Qt::Key_Up: return 0x26;
			case Qt::Key_Right: return 0x27;
			case Qt::Key_Down: return 0x28;
			case Qt::Key_Insert: return 0x2D;
			case Qt::Key_Delete: return 0x2E;
			case Qt::Key_0: return 0x30;
			case Qt::Key_1: return 0x31;
			case Qt::Key_2: return 0x32;
			case Qt::Key_3: return 0x33;
			case Qt::Key_4: return 0x34;
			case Qt::Key_5: return 0x35;
			case Qt::Key_6: return 0x36;
			case Qt::Key_7: return 0x37;
			case Qt::Key_8: return 0x38;
			case Qt::Key_9: return 0x39;
			case Qt::Key_A: return 0x41;
			case Qt::Key_B: return 0x42;
			case Qt::Key_C: return 0x43;
			case Qt::Key_D: return 0x44;
			case Qt::Key_E: return 0x45;
			case Qt::Key_F: return 0x46;
			case Qt::Key_G: return 0x47;
			case Qt::Key_H: return 0x48;
			case Qt::Key_I: return 0x49;
			case Qt::Key_J: return 0x4A;
			case Qt::Key_K: return 0x4B;
			case Qt::Key_L: return 0x4C;
			case Qt::Key_M: return 0x4D;
			case Qt::Key_N: return 0x4E;
			case Qt::Key_O: return 0x4F;
			case Qt::Key_P: return 0x50;
			case Qt::Key_Q: return 0x51;
			case Qt::Key_R: return 0x52;
			case Qt::Key_S: return 0x53;
			case Qt::Key_T: return 0x54;
			case Qt::Key_U: return 0x55;
			case Qt::Key_V: return 0x56;
			case Qt::Key_W: return 0x57;
			case Qt::Key_X: return 0x58;
			case Qt::Key_Y: return 0x59;
			case Qt::Key_Z: return 0x5A;
			case Qt::Key_F1: return 0x70;
			case Qt::Key_F2: return 0x71;
			case Qt::Key_F3: return 0x72;
			case Qt::Key_F4: return 0x73;
			case Qt::Key_F5: return 0x74;
			case Qt::Key_F6: return 0x75;
			case Qt::Key_F7: return 0x76;
			case Qt::Key_F8: return 0x77;
			case Qt::Key_F9: return 0x78;
			case Qt::Key_F10: return 0x79;
			case Qt::Key_F11: return 0x7A;
			case Qt::Key_F12: return 0x7B;
			default: break;
		}

		if (event->nativeVirtualKey() != 0)
		{
			return static_cast<int>(event->nativeVirtualKey());
		}

		return event->key();
	}

	bool isDarkPalette(const QPalette& palette)
	{
		const QColor baseColor = palette.color(QPalette::Base);
		const QColor textColor = palette.color(QPalette::Text);
		return baseColor.lightnessF() < textColor.lightnessF();
	}

	QString cssColor(const QColor& color)
	{
		return color.name(QColor::HexRgb);
	}

	cef_color_t toCefColor(const QColor& color)
	{
		return CefColorSetARGB(static_cast<uint8_t>(color.alpha()),
							   static_cast<uint8_t>(color.red()),
							   static_cast<uint8_t>(color.green()),
							   static_cast<uint8_t>(color.blue()));
	}

	QString launcherThemeBridgeScript(const QPalette& palette)
	{
		const bool prefersDark = isDarkPalette(palette);
		const QString scheme = prefersDark ? QStringLiteral("dark") : QStringLiteral("light");
		const QString baseColor = cssColor(palette.color(QPalette::Base));
		const QString textColor = cssColor(palette.color(QPalette::Text));
		const QString accentColor = cssColor(palette.color(QPalette::Highlight));
		const QString surfaceColor = cssColor(palette.color(QPalette::AlternateBase));

		return QStringLiteral(R"JS(
(() => {
  const theme = {
    prefersDark: %1,
    scheme: '%2',
    baseColor: '%3',
    textColor: '%4',
    accentColor: '%5',
    surfaceColor: '%6'
  };
  window.__projtLauncherTheme = theme;

  const mountPoint = document.head || document.documentElement || document.body;
  if (mountPoint) {
    let style = document.getElementById('projt-launcher-theme-bridge');
    if (!style) {
      style = document.createElement('style');
      style.id = 'projt-launcher-theme-bridge';
      mountPoint.appendChild(style);
    }
    style.textContent = `
      :root {
        color-scheme: ${theme.scheme};
        accent-color: ${theme.accentColor};
        scrollbar-color: ${theme.accentColor} ${theme.surfaceColor};
        --projt-launcher-base: ${theme.baseColor};
        --projt-launcher-text: ${theme.textColor};
        --projt-launcher-accent: ${theme.accentColor};
        --projt-launcher-surface: ${theme.surfaceColor};
      }
      html, body {
        background: ${theme.baseColor} !important;
        color: ${theme.textColor} !important;
      }
      input, textarea, select, button {
        color-scheme: ${theme.scheme} !important;
      }
      ::selection {
        background: ${theme.accentColor};
      }
    `;
  }

  if (document.documentElement) {
    document.documentElement.style.colorScheme = theme.scheme;
    document.documentElement.style.backgroundColor = theme.baseColor;
    document.documentElement.style.color = theme.textColor;
    document.documentElement.dataset.projtLauncherScheme = theme.scheme;
  }

  if (document.body) {
    document.body.style.backgroundColor = theme.baseColor;
    document.body.style.color = theme.textColor;
    document.body.style.accentColor = theme.accentColor;
  }

  let themeColorMeta = document.querySelector('meta[name="theme-color"]');
  if (!themeColorMeta && document.head) {
    themeColorMeta = document.createElement('meta');
    themeColorMeta.name = 'theme-color';
    document.head.appendChild(themeColorMeta);
  }
  if (themeColorMeta) {
    themeColorMeta.content = theme.baseColor;
  }

  let colorSchemeMeta = document.querySelector('meta[name="color-scheme"]');
  if (!colorSchemeMeta && document.head) {
    colorSchemeMeta = document.createElement('meta');
    colorSchemeMeta.name = 'color-scheme';
    document.head.appendChild(colorSchemeMeta);
  }
  if (colorSchemeMeta) {
    colorSchemeMeta.content = theme.prefersDark ? 'dark light' : 'light dark';
  }

  if (window.matchMedia && !window.__projtLauncherMatchMediaPatched) {
    const originalMatchMedia = window.matchMedia.bind(window);
    const createList = (query, matches) => {
      const listeners = new Set();
      return {
        matches,
        media: query,
        onchange: null,
        addListener(listener) {
          if (typeof listener === 'function') listeners.add(listener);
        },
        removeListener(listener) {
          listeners.delete(listener);
        },
        addEventListener(type, listener) {
          if (type === 'change' && typeof listener === 'function') listeners.add(listener);
        },
        removeEventListener(type, listener) {
          if (type === 'change') listeners.delete(listener);
        },
        dispatchEvent(event) {
          listeners.forEach((listener) => listener(event));
          if (typeof this.onchange === 'function') this.onchange(event);
          return true;
        }
      };
    };

    window.matchMedia = (query) => {
      const normalized = String(query).trim().toLowerCase();
      if (normalized.includes('prefers-color-scheme')) {
        const currentTheme = window.__projtLauncherTheme || { prefersDark: false };
        const matches = (normalized.includes('dark') && currentTheme.prefersDark)
          || (normalized.includes('light') && !currentTheme.prefersDark);
        return createList(query, matches);
      }
      return originalMatchMedia(query);
    };

    Object.defineProperty(window, '__projtLauncherMatchMediaPatched', {
      value: true,
      configurable: true
    });
  }
})();
)JS")
			.arg(prefersDark ? QStringLiteral("true") : QStringLiteral("false"),
				 scheme,
				 baseColor,
				 textColor,
				 accentColor,
				 surfaceColor);
	}

	class CefHubClient final : public CefClient,
							   public CefDisplayHandler,
							   public CefDialogHandler,
							   public CefJSDialogHandler,
							   public CefLifeSpanHandler,
							   public CefLoadHandler,
							   public CefPermissionHandler,
							   public CefRenderHandler,
							   public CefRequestHandler
	{
	  public:
		explicit CefHubClient(CefHubView* owner) : m_owner(owner)
		{}

		bool isPrimaryBrowser(CefRefPtr<CefBrowser> browser) const
		{
			return browser && m_browser && browser->GetIdentifier() == m_browser->GetIdentifier();
		}

		bool isUnexpectedSecondaryBrowser(CefRefPtr<CefBrowser> browser) const
		{
			return browser && m_browser && browser->GetIdentifier() != m_browser->GetIdentifier();
		}

		void queuePopupRequest(const QUrl& url)
		{
			if (!m_owner || !url.isValid() || url.isEmpty() || url == QUrl(QStringLiteral("about:blank")))
			{
				return;
			}

			QMetaObject::invokeMethod(
				m_owner,
				[owner = m_owner, url]() { owner->handlePopupRequest(url); },
				Qt::QueuedConnection);
		}

		CefRefPtr<CefDisplayHandler> GetDisplayHandler() override
		{
			return this;
		}

		CefRefPtr<CefDialogHandler> GetDialogHandler() override
		{
			return this;
		}

		CefRefPtr<CefJSDialogHandler> GetJSDialogHandler() override
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

		CefRefPtr<CefPermissionHandler> GetPermissionHandler() override
		{
			return this;
		}

		CefRefPtr<CefRenderHandler> GetRenderHandler() override
		{
			return this;
		}

		CefRefPtr<CefRequestHandler> GetRequestHandler() override
		{
			return this;
		}

		bool GetRootScreenRect(CefRefPtr<CefBrowser>, CefRect& rect) override
		{
			if (!m_owner)
			{
				return false;
			}

			const QRect globalRect(m_owner->mapToGlobal(QPoint(0, 0)), m_owner->size());
			rect = CefRect(globalRect.x(),
						   globalRect.y(),
						   std::max(1, globalRect.width()),
						   std::max(1, globalRect.height()));
			return true;
		}

		void GetViewRect(CefRefPtr<CefBrowser>, CefRect& rect) override
		{
			if (!m_owner)
			{
				rect = CefRect(0, 0, 1, 1);
				return;
			}

			rect = CefRect(0, 0, std::max(1, m_owner->width()), std::max(1, m_owner->height()));
		}

		bool GetScreenPoint(CefRefPtr<CefBrowser>, int viewX, int viewY, int& screenX, int& screenY) override
		{
			if (!m_owner)
			{
				return false;
			}

			const QPoint globalPoint = m_owner->mapToGlobal(QPoint(viewX, viewY));
			screenX					 = globalPoint.x();
			screenY					 = globalPoint.y();
			return true;
		}

		bool GetScreenInfo(CefRefPtr<CefBrowser>, CefScreenInfo& screen_info) override
		{
			if (!m_owner)
			{
				return false;
			}

			const QRect globalRect(m_owner->mapToGlobal(QPoint(0, 0)), m_owner->size());
			const qreal scaleFactor = m_owner->devicePixelRatioF();

			screen_info.device_scale_factor = scaleFactor;
			screen_info.depth				= 32;
			screen_info.depth_per_component = 8;
			screen_info.is_monochrome		= false;
			screen_info.rect				= CefRect(globalRect.x(),
										  globalRect.y(),
										  std::max(1, globalRect.width()),
										  std::max(1, globalRect.height()));
			screen_info.available_rect		= screen_info.rect;
			return true;
		}

		void OnPopupShow(CefRefPtr<CefBrowser>, bool show) override
		{
			if (m_owner)
			{
				m_owner->handlePopupVisibility(show);
			}
		}

		void OnPopupSize(CefRefPtr<CefBrowser>, const CefRect& rect) override
		{
			if (m_owner)
			{
				m_owner->handlePopupRect(QRect(rect.x, rect.y, rect.width, rect.height));
			}
		}

		void OnPaint(CefRefPtr<CefBrowser>,
					 PaintElementType type,
					 const RectList& dirtyRects,
					 const void* buffer,
					 int width,
					 int height) override
		{
			if (!m_owner || !buffer || width <= 0 || height <= 0)
			{
				return;
			}

			QImage image(static_cast<const uchar*>(buffer), width, height, QImage::Format_ARGB32);
			QImage copy = image.copy();
			copy.setDevicePixelRatio(m_owner->devicePixelRatioF());

			QVector<QRect> qtDirtyRects;
			qtDirtyRects.reserve(static_cast<qsizetype>(dirtyRects.size()));
			for (const auto& rect : dirtyRects)
			{
				qtDirtyRects.append(QRect(rect.x, rect.y, rect.width, rect.height));
			}

			m_owner->handlePaint(type == PET_POPUP, copy, qtDirtyRects);
		}

		void OnAddressChange(CefRefPtr<CefBrowser> cefBrowser, CefRefPtr<CefFrame> frame, const CefString& url) override
		{
			if (!m_owner || !frame || !frame->IsMain())
			{
				return;
			}
			const QUrl qtUrl(QString::fromStdString(url.ToString()));
			if (cefBrowser && (cefBrowser->IsPopup() || isUnexpectedSecondaryBrowser(cefBrowser)))
			{
				queuePopupRequest(qtUrl);
				cefBrowser->GetHost()->CloseBrowser(true);
				return;
			}
			if (cefBrowser && !isPrimaryBrowser(cefBrowser))
			{
				return;
			}
			QMetaObject::invokeMethod(
				m_owner,
				[owner = m_owner, qtUrl]() { owner->handleAddressChange(qtUrl); },
				Qt::QueuedConnection);
		}

		void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) override
		{
			if (browser && (browser->IsPopup() || !isPrimaryBrowser(browser)))
			{
				return;
			}
			const QString qtTitle = QString::fromStdString(title.ToString());
			QMetaObject::invokeMethod(
				m_owner,
				[owner = m_owner, qtTitle]() { owner->handleTitleChange(qtTitle); },
				Qt::QueuedConnection);
		}

		void OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
								  bool isLoading,
								  bool canGoBack,
								  bool canGoForward) override
		{
			if (browser && browser->IsPopup())
			{
				return;
			}
			if (browser && isUnexpectedSecondaryBrowser(browser))
			{
				if (!isLoading)
				{
					const QUrl qtUrl(QString::fromStdString(browser->GetMainFrame()->GetURL().ToString()));
					queuePopupRequest(qtUrl);
					browser->GetHost()->CloseBrowser(true);
				}
				return;
			}
			if (browser && !isPrimaryBrowser(browser))
			{
				return;
			}
			QMetaObject::invokeMethod(
				m_owner,
				[owner = m_owner, isLoading, canGoBack, canGoForward]()
				{ owner->handleLoadingState(isLoading, canGoBack, canGoForward); },
				Qt::QueuedConnection);
		}

		void OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, TransitionType) override
		{
			if (!m_owner || !frame || !frame->IsMain())
			{
				return;
			}
			if (browser && (browser->IsPopup() || !isPrimaryBrowser(browser)))
			{
				return;
			}

			m_owner->applyLauncherThemeToFrame(frame);
		}

		void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int) override
		{
			if (!m_owner || !frame || !frame->IsMain())
			{
				return;
			}
			if (browser && (browser->IsPopup() || !isPrimaryBrowser(browser)))
			{
				return;
			}
			QMetaObject::invokeMethod(
				m_owner,
				[owner = m_owner]() { owner->handleLoadFinished(true); },
				Qt::QueuedConnection);
		}

		void OnLoadError(CefRefPtr<CefBrowser> browser,
						 CefRefPtr<CefFrame> frame,
						 ErrorCode,
						 const CefString&,
						 const CefString&) override
		{
			if (!m_owner || !frame || !frame->IsMain())
			{
				return;
			}
			if (browser && (browser->IsPopup() || !isPrimaryBrowser(browser)))
			{
				return;
			}
			QMetaObject::invokeMethod(
				m_owner,
				[owner = m_owner]() { owner->handleLoadFinished(false); },
				Qt::QueuedConnection);
		}

		void OnAfterCreated(CefRefPtr<CefBrowser> browser) override
		{
			if (!browser)
			{
				return;
			}

			if (!m_browser)
			{
				m_browser = browser;
				return;
			}

			if (browser->IsPopup() || isUnexpectedSecondaryBrowser(browser))
			{
				if (m_pendingPopupUrl.isValid())
				{
					const QUrl qtUrl  = m_pendingPopupUrl;
					m_pendingPopupUrl = QUrl();
					queuePopupRequest(qtUrl);
				}
				else
				{
					const QUrl qtUrl(QString::fromStdString(browser->GetMainFrame()->GetURL().ToString()));
					queuePopupRequest(qtUrl);
				}
				browser->GetHost()->CloseBrowser(true);
			}
		}

		bool OnBeforePopup(CefRefPtr<CefBrowser>,
						   CefRefPtr<CefFrame>,
						   int,
						   const CefString& target_url,
						   const CefString&,
						   cef_window_open_disposition_t,
						   bool,
						   const CefPopupFeatures&,
						   CefWindowInfo&,
						   CefRefPtr<CefClient>&,
						   CefBrowserSettings&,
						   CefRefPtr<CefDictionaryValue>&,
						   bool*) override
		{
			if (!m_owner)
			{
				return true;
			}

			const QUrl qtUrl(QString::fromStdString(target_url.ToString()));
			if (qtUrl.isValid() && !qtUrl.isEmpty())
			{
				m_pendingPopupUrl = qtUrl;
				queuePopupRequest(qtUrl);
			}
			return true;
		}

		bool OnOpenURLFromTab(CefRefPtr<CefBrowser>,
							  CefRefPtr<CefFrame>,
							  const CefString& target_url,
							  cef_window_open_disposition_t target_disposition,
							  bool) override
		{
			if (!m_owner)
			{
				return false;
			}

			switch (target_disposition)
			{
				case CEF_WOD_SINGLETON_TAB:
				case CEF_WOD_NEW_FOREGROUND_TAB:
				case CEF_WOD_NEW_BACKGROUND_TAB:
				case CEF_WOD_SWITCH_TO_TAB:
				case CEF_WOD_NEW_POPUP:
				case CEF_WOD_NEW_WINDOW:
				case CEF_WOD_OFF_THE_RECORD:
				case CEF_WOD_UNKNOWN:
				{
					const QUrl qtUrl(QString::fromStdString(target_url.ToString()));
					if (qtUrl.isValid() && !qtUrl.isEmpty())
					{
						m_pendingPopupUrl = qtUrl;
						queuePopupRequest(qtUrl);
						return true;
					}
					break;
				}
				default: break;
			}

			return false;
		}

		bool OnFileDialog(CefRefPtr<CefBrowser>,
						  FileDialogMode,
						  const CefString& title,
						  const CefString&,
						  const std::vector<CefString>&,
						  const std::vector<CefString>&,
						  const std::vector<CefString>&,
						  CefRefPtr<CefFileDialogCallback> callback) override
		{
			qWarning() << "[LauncherHub][CEF] Blocked file dialog:" << QString::fromStdString(title.ToString());
			if (callback)
			{
				callback->Cancel();
			}
			return true;
		}

		bool OnJSDialog(CefRefPtr<CefBrowser>,
						const CefString& origin_url,
						JSDialogType,
						const CefString& message_text,
						const CefString&,
						CefRefPtr<CefJSDialogCallback> callback,
						bool& suppress_message) override
		{
			qWarning() << "[LauncherHub][CEF] Blocked JS dialog from" << QString::fromStdString(origin_url.ToString())
					   << ":" << QString::fromStdString(message_text.ToString());
			suppress_message = true;
			if (callback)
			{
				callback->Continue(false, CefString());
			}
			return true;
		}

		bool OnBeforeUnloadDialog(CefRefPtr<CefBrowser>,
								  const CefString& message_text,
								  bool,
								  CefRefPtr<CefJSDialogCallback> callback) override
		{
			qWarning() << "[LauncherHub][CEF] Auto-accepted beforeunload dialog:"
					   << QString::fromStdString(message_text.ToString());
			if (callback)
			{
				callback->Continue(true, CefString());
			}
			return true;
		}

		bool OnRequestMediaAccessPermission(CefRefPtr<CefBrowser>,
											CefRefPtr<CefFrame>,
											const CefString& requesting_origin,
											uint32_t,
											CefRefPtr<CefMediaAccessCallback> callback) override
		{
			qWarning() << "[LauncherHub][CEF] Blocked media permission request from"
					   << QString::fromStdString(requesting_origin.ToString());
			if (callback)
			{
				callback->Cancel();
			}
			return true;
		}

		bool OnShowPermissionPrompt(CefRefPtr<CefBrowser>,
									uint64_t,
									const CefString& requesting_origin,
									uint32_t,
									CefRefPtr<CefPermissionPromptCallback> callback) override
		{
			qWarning() << "[LauncherHub][CEF] Blocked permission prompt from"
					   << QString::fromStdString(requesting_origin.ToString());
			if (callback)
			{
				callback->Continue(CEF_PERMISSION_RESULT_DENY);
			}
			return true;
		}

		void OnBeforeClose(CefRefPtr<CefBrowser> browser) override
		{
			if (browser && browser->IsPopup())
			{
				return;
			}
			if (browser && isUnexpectedSecondaryBrowser(browser))
			{
				return;
			}
			QMetaObject::invokeMethod(
				m_owner,
				[owner = m_owner]() { owner->handleBrowserClosed(); },
				Qt::QueuedConnection);
			m_browser = nullptr;
		}

		CefRefPtr<CefBrowser> browser() const
		{
			return m_browser;
		}

		void detachOwner()
		{
			m_owner = nullptr;
		}

		IMPLEMENT_REFCOUNTING(CefHubClient);

	  private:
		CefHubView* m_owner = nullptr;
		CefRefPtr<CefBrowser> m_browser;
		QUrl m_pendingPopupUrl;
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
	setAttribute(Qt::WA_InputMethodEnabled);
	setFocusPolicy(Qt::StrongFocus);
	setMouseTracking(true);
}

CefHubView::~CefHubView()
{
	if (m_impl && m_impl->client)
	{
		m_impl->client->detachOwner();
		if (m_impl->client->browser() && !m_closing)
		{
			m_closing = true;
			m_impl->client->browser()->GetHost()->CloseBrowser(true);
		}
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

void CefHubView::setActive(bool active)
{
	m_active = active;
	if (m_impl && m_impl->client && m_impl->client->browser())
	{
		auto host = m_impl->client->browser()->GetHost();
		host->WasHidden(!m_active || isHidden());
		if (m_active && !isHidden())
		{
			host->Invalidate(PET_VIEW);
		}
	}
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

void CefHubView::paintEvent(QPaintEvent* event)
{
	Q_UNUSED(event);

	QPainter painter(this);
	painter.fillRect(rect(), palette().brush(QPalette::Base));

	if (!m_viewImage.isNull())
	{
		painter.drawImage(QPoint(0, 0), m_viewImage);
	}

	if (m_popupVisible && !m_popupImage.isNull())
	{
		painter.drawImage(m_popupRect.topLeft(), m_popupImage);
	}
}

void CefHubView::changeEvent(QEvent* event)
{
	HubViewBase::changeEvent(event);
	if (!event)
	{
		return;
	}

	if (event->type() == QEvent::PaletteChange || event->type() == QEvent::ApplicationPaletteChange)
	{
		applyLauncherTheme();
	}
}

void CefHubView::resizeEvent(QResizeEvent* event)
{
	HubViewBase::resizeEvent(event);
	if (m_impl && m_impl->client && m_impl->client->browser())
	{
		auto host = m_impl->client->browser()->GetHost();
		host->NotifyMoveOrResizeStarted();
		host->NotifyScreenInfoChanged();
		host->WasResized();
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
	if (m_impl && m_impl->client && m_impl->client->browser())
	{
		m_impl->client->browser()->GetHost()->WasHidden(!m_active);
	}
}

void CefHubView::hideEvent(QHideEvent* event)
{
	HubViewBase::hideEvent(event);
	if (m_impl && m_impl->client && m_impl->client->browser())
	{
		m_impl->client->browser()->GetHost()->WasHidden(true);
	}
}

void CefHubView::focusInEvent(QFocusEvent* event)
{
	HubViewBase::focusInEvent(event);
	if (m_impl && m_impl->client && m_impl->client->browser())
	{
		m_impl->client->browser()->GetHost()->SetFocus(true);
	}
}

void CefHubView::focusOutEvent(QFocusEvent* event)
{
	HubViewBase::focusOutEvent(event);
	if (m_impl && m_impl->client && m_impl->client->browser())
	{
		m_impl->client->browser()->GetHost()->SetFocus(false);
	}
}

void CefHubView::mouseMoveEvent(QMouseEvent* event)
{
	HubViewBase::mouseMoveEvent(event);
	if (!(m_impl && m_impl->client && m_impl->client->browser()))
	{
		return;
	}

	m_impl->client->browser()->GetHost()->SendMouseMoveEvent(
		toCefMouseEvent(event->position(), event->modifiers(), event->buttons()),
		false);
}

void CefHubView::mousePressEvent(QMouseEvent* event)
{
	HubViewBase::mousePressEvent(event);
	if (!(m_impl && m_impl->client && m_impl->client->browser()))
	{
		return;
	}

	setFocus(Qt::MouseFocusReason);
	auto host = m_impl->client->browser()->GetHost();
	host->SetFocus(true);
	host->SendMouseClickEvent(toCefMouseEvent(event->position(), event->modifiers(), event->buttons()),
							  toCefMouseButton(event->button()),
							  false,
							  1);
}

void CefHubView::mouseReleaseEvent(QMouseEvent* event)
{
	HubViewBase::mouseReleaseEvent(event);
	if (!(m_impl && m_impl->client && m_impl->client->browser()))
	{
		return;
	}

	m_impl->client->browser()->GetHost()->SendMouseClickEvent(
		toCefMouseEvent(event->position(), event->modifiers(), event->buttons()),
		toCefMouseButton(event->button()),
		true,
		1);
}

void CefHubView::wheelEvent(QWheelEvent* event)
{
	HubViewBase::wheelEvent(event);
	if (!(m_impl && m_impl->client && m_impl->client->browser()))
	{
		return;
	}

	const QPoint angleDelta = event->angleDelta();
	m_impl->client->browser()->GetHost()->SendMouseWheelEvent(
		toCefMouseEvent(event->position(), event->modifiers(), event->buttons()),
		angleDelta.x(),
		angleDelta.y());
}

void CefHubView::keyPressEvent(QKeyEvent* event)
{
	HubViewBase::keyPressEvent(event);
	if (!(m_impl && m_impl->client && m_impl->client->browser()))
	{
		return;
	}

	CefKeyEvent keyEvent;
	keyEvent.type	   = KEYEVENT_RAWKEYDOWN;
	keyEvent.modifiers = toCefModifiers(event->modifiers(), QApplication::mouseButtons());
	if (event->isAutoRepeat())
	{
		keyEvent.modifiers |= EVENTFLAG_IS_REPEAT;
	}
	keyEvent.windows_key_code		 = toWindowsKeyCode(event);
	keyEvent.native_key_code		 = static_cast<int>(event->nativeScanCode());
	keyEvent.is_system_key			 = 0;
	keyEvent.character				 = event->text().isEmpty() ? 0 : event->text().at(0).unicode();
	keyEvent.unmodified_character	 = keyEvent.character;
	keyEvent.focus_on_editable_field = 0;

	auto host = m_impl->client->browser()->GetHost();
	host->SendKeyEvent(keyEvent);

	if (!event->text().isEmpty())
	{
		CefKeyEvent charEvent = keyEvent;
		charEvent.type		  = KEYEVENT_CHAR;
		host->SendKeyEvent(charEvent);
	}
}

void CefHubView::keyReleaseEvent(QKeyEvent* event)
{
	HubViewBase::keyReleaseEvent(event);
	if (!(m_impl && m_impl->client && m_impl->client->browser()))
	{
		return;
	}

	CefKeyEvent keyEvent;
	keyEvent.type					 = KEYEVENT_KEYUP;
	keyEvent.modifiers				 = toCefModifiers(event->modifiers(), QApplication::mouseButtons());
	keyEvent.windows_key_code		 = toWindowsKeyCode(event);
	keyEvent.native_key_code		 = static_cast<int>(event->nativeScanCode());
	keyEvent.is_system_key			 = 0;
	keyEvent.character				 = event->text().isEmpty() ? 0 : event->text().at(0).unicode();
	keyEvent.unmodified_character	 = keyEvent.character;
	keyEvent.focus_on_editable_field = 0;

	m_impl->client->browser()->GetHost()->SendKeyEvent(keyEvent);
}

void CefHubView::leaveEvent(QEvent* event)
{
	HubViewBase::leaveEvent(event);
	if (!(m_impl && m_impl->client && m_impl->client->browser()))
	{
		return;
	}

	m_impl->client->browser()->GetHost()->SendMouseMoveEvent(
		toCefMouseEvent(QPointF(-1, -1), Qt::NoModifier, Qt::NoButton),
		true);
}

void CefHubView::ensureBrowser()
{
	if (m_created || width() <= 0 || height() <= 0 || !projt::cef::Runtime::instance().isInitialized())
	{
		return;
	}

	m_impl->client = new CefHubClient(this);

	CefWindowInfo windowInfo;
	windowInfo.SetAsWindowless(static_cast<CefWindowHandle>(winId()));

	CefBrowserSettings browserSettings;
	browserSettings.windowless_frame_rate = 60;
	const QColor baseColor = palette().color(QPalette::Base);
	browserSettings.background_color = toCefColor(baseColor);
	const QString initialUrl			  = m_url.isValid() ? m_url.toString() : QStringLiteral("about:blank");
	auto browser						  = CefBrowserHost::CreateBrowserSync(windowInfo,
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
	browser->GetHost()->SetWindowlessFrameRate(60);
	browser->GetHost()->WasHidden(!m_active || isHidden());
	browser->GetHost()->NotifyScreenInfoChanged();
	applyLauncherTheme();
	browser->GetHost()->Invalidate(PET_VIEW);
	syncNavigationState();
}

void CefHubView::syncNavigationState()
{
	if (!m_impl || !m_impl->client || !m_impl->client->browser())
	{
		return;
	}

	const bool oldCanGoBack	   = m_canGoBack;
	const bool oldCanGoForward = m_canGoForward;
	m_canGoBack				   = m_impl->client->browser()->CanGoBack();
	m_canGoForward			   = m_impl->client->browser()->CanGoForward();
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
	m_canGoBack	   = canGoBack;
	m_canGoForward = canGoForward;
	emit navigationStateChanged();
}

void CefHubView::handleLoadFinished(bool ok)
{
	applyLauncherTheme();
	syncNavigationState();
	emit loadFinished(ok);
}

void CefHubView::handleBrowserClosed()
{
	m_created = false;
	m_closing = true;
	emit navigationStateChanged();
}

void CefHubView::handlePopupRequest(const QUrl& url)
{
	if (!url.isValid())
	{
		return;
	}

	emit newTabRequested(url);
}

void CefHubView::handlePopupVisibility(bool visible)
{
	m_popupVisible = visible;
	if (!m_popupVisible)
	{
		m_popupImage = QImage();
		m_popupRect	 = QRect();
	}
	update();
}

void CefHubView::handlePopupRect(const QRect& rect)
{
	m_popupRect = rect;
	update();
}

void CefHubView::handlePaint(bool popup, const QImage& image, const QVector<QRect>& dirtyRects)
{
	Q_UNUSED(dirtyRects);

	if (popup)
	{
		m_popupImage = image;
	}
	else
	{
		m_viewImage = image;
	}

	update();
}

void CefHubView::applyLauncherTheme()
{
	if (!(m_impl && m_impl->client && m_impl->client->browser()))
	{
		return;
	}

	applyLauncherThemeToFrame(m_impl->client->browser()->GetMainFrame());

	auto requestContext = m_impl->client->browser()->GetHost()->GetRequestContext();
	if (!requestContext)
	{
		return;
	}

	const cef_color_variant_t variant =
		isDarkPalette(palette()) ? CEF_COLOR_VARIANT_DARK : CEF_COLOR_VARIANT_LIGHT;
	requestContext->SetChromeColorScheme(variant, toCefColor(palette().color(QPalette::Highlight)));
}

void CefHubView::applyLauncherThemeToFrame(CefRefPtr<CefFrame> frame)
{
	if (!frame)
	{
		return;
	}

	frame->ExecuteJavaScript(launcherThemeBridgeScript(palette()).toStdString(), frame->GetURL(), 0);
}

#endif
