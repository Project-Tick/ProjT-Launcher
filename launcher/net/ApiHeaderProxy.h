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
 *
 * === Upstream License Block (Do Not Modify) ==============================
 *
 *
 *
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (C) 2023 Rachel Powers <508861+Ryex@users.noreply.github.com>
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
 * ======================================================================== */

#pragma once

#include "Application.h"
#include "BuildConfig.h"
#include "net/HeaderProxy.h"

namespace Net
{

	class ApiHeaderProxy : public HeaderProxy
	{
	  public:
		ApiHeaderProxy() : HeaderProxy()
		{}
		virtual ~ApiHeaderProxy() = default;

	  public:
		virtual QList<HeaderPair> headers(const QNetworkRequest& request) const override
		{
			QList<HeaderPair> hdrs;
			if (APPLICATION->capabilities() & Application::SupportsFlame
				&& request.url().host() == QUrl(BuildConfig.FLAME_BASE_URL).host())
			{
				hdrs.append({ "x-api-key", APPLICATION->getFlameAPIKey().toUtf8() });
			}
			else if (request.url().host() == QUrl(BuildConfig.MODRINTH_PROD_URL).host()
					 || request.url().host() == QUrl(BuildConfig.MODRINTH_STAGING_URL).host())
			{
				QString token = APPLICATION->getModrinthAPIToken();
				if (!token.isNull())
					hdrs.append({ "Authorization", token.toUtf8() });
			}
			return hdrs;
		};
	};

} // namespace Net
