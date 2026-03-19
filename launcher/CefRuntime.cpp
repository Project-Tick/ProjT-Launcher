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

#include "CefRuntime.h"

#if defined(PROJT_USE_CEF) && defined(Q_OS_LINUX)

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QProcessEnvironment>
#include <QStandardPaths>
#include <QTimer>

#include "include/cef_app.h"
#include "include/wrapper/cef_helpers.h"

namespace
{
	class LauncherCefApp final : public CefApp
	{
	  public:
		void OnBeforeCommandLineProcessing(const CefString&, CefRefPtr<CefCommandLine> command_line) override
		{
			command_line->AppendSwitch("disable-pinch");
			command_line->AppendSwitch("disable-smooth-scrolling");
			command_line->AppendSwitch("disable-background-networking");
			command_line->AppendSwitch("disable-background-mode");
			command_line->AppendSwitch("disable-component-update");
			command_line->AppendSwitch("disable-sync");
			command_line->AppendSwitch("disable-notifications");
			command_line->AppendSwitch("no-first-run");

			QString disabledFeatures = QStringLiteral("PlatformNotifications,PushMessaging,NotificationTriggers");

			const bool enableGpu = qEnvironmentVariableIntValue("PROJT_CEF_ENABLE_GPU") == 1
								|| qEnvironmentVariableIntValue("LAUNCHER_CEF_ENABLE_GPU") == 1;
			if (!enableGpu)
			{
				command_line->AppendSwitch("disable-gpu");
				command_line->AppendSwitch("disable-gpu-compositing");
				command_line->AppendSwitch("disable-gpu-vsync");
				disabledFeatures += QStringLiteral(",VaapiVideoDecoder,Vulkan");
			}

			command_line->AppendSwitchWithValue("disable-features", disabledFeatures.toStdString());
		}

		IMPLEMENT_REFCOUNTING(LauncherCefApp);
	};
}

#endif

namespace projt::cef
{
	Runtime::Runtime(QObject* parent) : QObject(parent)
	{}

	Runtime& Runtime::instance()
	{
		static Runtime runtime;
		return runtime;
	}

	int Runtime::executeSecondaryProcess(int argc, char* argv[])
	{
#if defined(PROJT_USE_CEF) && defined(Q_OS_LINUX)
		CefMainArgs mainArgs(argc, argv);
		return CefExecuteProcess(mainArgs, new LauncherCefApp(), nullptr);
#else
		Q_UNUSED(argc);
		Q_UNUSED(argv);
		return -1;
#endif
	}

	bool Runtime::initializeBrowserProcess(int argc, char* argv[])
	{
#if defined(PROJT_USE_CEF) && defined(Q_OS_LINUX)
		if (m_initialized)
		{
			return true;
		}

		CefMainArgs mainArgs(argc, argv);
		CefSettings settings;
		settings.no_sandbox					  = true;
		settings.multi_threaded_message_loop  = false;
		settings.external_message_pump		  = false;
		settings.windowless_rendering_enabled = true;

		const QString executablePath = QCoreApplication::applicationFilePath();
		const QString executableDir	 = QFileInfo(executablePath).absolutePath();
		const QString dataRoot =
			QDir::cleanPath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/cef");
		QDir().mkpath(dataRoot);

		CefString(&settings.browser_subprocess_path) = executablePath.toStdString();
		CefString(&settings.resources_dir_path)		 = executableDir.toStdString();
		CefString(&settings.locales_dir_path)		 = QDir(executableDir).filePath("locales").toStdString();
		CefString(&settings.root_cache_path)		 = dataRoot.toStdString();
		CefString(&settings.cache_path)				 = QDir(dataRoot).filePath("cache").toStdString();
		CefString(&settings.user_agent_product)		 = "ProjTLauncher";

		const bool ok = CefInitialize(mainArgs, settings, new LauncherCefApp(), nullptr);
		m_exitCode	  = ok ? 0 : CefGetExitCode();
		if (!ok)
		{
			if (m_exitCode == 0)
			{
				m_exitCode = 1;
			}
			return false;
		}

		auto* timer = new QTimer(this);
		timer->setInterval(10);
		timer->setTimerType(Qt::PreciseTimer);
		connect(timer, &QTimer::timeout, this, []() { CefDoMessageLoopWork(); });
		timer->start();

		m_initialized = true;
		return true;
#else
		Q_UNUSED(argc);
		Q_UNUSED(argv);
		return false;
#endif
	}

	void Runtime::shutdown()
	{
#if defined(PROJT_USE_CEF) && defined(Q_OS_LINUX)
		if (!m_initialized)
		{
			return;
		}

		for (auto* timer : findChildren<QTimer*>())
		{
			timer->stop();
			timer->deleteLater();
		}
		CefShutdown();
		m_initialized = false;
#endif
	}

	bool Runtime::isInitialized() const
	{
		return m_initialized;
	}

	int Runtime::exitCode() const
	{
		return m_exitCode;
	}
}
