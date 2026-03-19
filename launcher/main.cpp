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
/* === Upstream License Block (Do Not Modify) ==============================
 *
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (C) 2022 Sefa Eyeoglu <contact@scrumplex.net>
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
 *
 * This file incorporates work covered by the following copyright and
 * permission notice:
 *
 *      Copyright 2013-2021 MultiMC Contributors
 *
 *      Licensed under the Apache License, Version 2.0 (the "License");
 *      you may not use this file except in compliance with the License.
 *      You may obtain a copy of the License at
 *
 *          http://www.apache.org/licenses/LICENSE-2.0
 *
 *      Unless required by applicable law or agreed to in writing, software
 *      distributed under the License is distributed on an "AS IS" BASIS,
 *      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *      See the License for the specific language governing permissions and
 *      limitations under the License.
 *
   ======================================================================== */

#include "Application.h"

#if defined(Q_OS_LINUX)
#include "CefRuntime.h"

#include <QByteArray>
#endif

int main(int argc, char* argv[])
{
#if defined(Q_OS_LINUX)
	// Prefer GTK-themed integration on GNOME if user hasn't set a platform theme.
	if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORMTHEME"))
	{
		const auto desktop = qgetenv("XDG_CURRENT_DESKTOP").toLower();
		if (desktop.contains("gnome"))
		{
			qputenv("QT_QPA_PLATFORMTHEME", "gnome");
		}
	}

	const QByteArray sessionType	  = qgetenv("XDG_SESSION_TYPE").toLower();
	const QByteArray qtPlatform		  = qgetenv("QT_QPA_PLATFORM").toLower();
	const bool nativeWaylandRequested = (sessionType == "wayland" || !qgetenv("WAYLAND_DISPLAY").isEmpty())
									 && (qtPlatform.isEmpty() || qtPlatform.startsWith("wayland"));
	if (nativeWaylandRequested)
	{
		qputenv("QT_QPA_PLATFORM", "xcb");
	}

#if defined(PROJT_USE_CEF)
	const int cefExitCode = projt::cef::Runtime::executeSecondaryProcess(argc, argv);
	if (cefExitCode >= 0)
	{
		return cefExitCode;
	}
#endif
#endif

	// initialize Qt
	Application app(argc, argv);

	switch (app.status())
	{
		case Application::StartingUp:
		case Application::Initialized:
		{
#if defined(Q_OS_LINUX) && defined(PROJT_USE_CEF)
			if (!projt::cef::Runtime::instance().initializeBrowserProcess(argc, argv))
			{
				return projt::cef::Runtime::instance().exitCode();
			}
#endif

			Q_INIT_RESOURCE(multimc);
			Q_INIT_RESOURCE(backgrounds);
			Q_INIT_RESOURCE(documents);
			Q_INIT_RESOURCE(projtlauncher);

			Q_INIT_RESOURCE(pe_dark);
			Q_INIT_RESOURCE(pe_light);
			Q_INIT_RESOURCE(pe_blue);
			Q_INIT_RESOURCE(pe_colored);
			Q_INIT_RESOURCE(breeze_dark);
			Q_INIT_RESOURCE(breeze_light);
			Q_INIT_RESOURCE(OSX);
			Q_INIT_RESOURCE(iOS);
			Q_INIT_RESOURCE(flat);
			Q_INIT_RESOURCE(flat_white);

			Q_INIT_RESOURCE(shaders);
			const int exitCode = app.exec();
#if defined(Q_OS_LINUX) && defined(PROJT_USE_CEF)
			projt::cef::Runtime::instance().shutdown();
#endif
			return exitCode;
		}
		case Application::Failed: return 1;
		case Application::Succeeded: return 0;
		default: return -1;
	}
}
