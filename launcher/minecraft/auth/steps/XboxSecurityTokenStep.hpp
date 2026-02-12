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

#include <QObject>
#include <QString>
#include <memory>

#include "Credentials.hpp"
#include "Step.hpp"
#include "net/NetJob.h"
#include "net/Upload.h"

namespace projt::minecraft::auth
{

	/**
	 * Target services for XSTS token requests.
	 */
	enum class XstsTarget
	{
		XboxLive,		  ///< Xbox Live services (for profile fetch)
		MinecraftServices ///< Minecraft services (for game access)
	};

	/**
	 * Xbox Security Token Service (XSTS) authorization step.
	 *
	 * Requests an XSTS token for a specific relying party (service).
	 * Two instances are typically used in the auth pipeline:
	 * - One for Xbox Live services (profile)
	 * - One for Minecraft services (game access)
	 *
	 * Endpoint: https://xsts.auth.xboxlive.com/xsts/authorize
	 */
	class XboxSecurityTokenStep : public Step
	{
		Q_OBJECT

	  public:
		/**
		 * Construct an XSTS authorization step.
		 * @param credentials Credentials to populate.
		 * @param target Which service to request authorization for.
		 */
		explicit XboxSecurityTokenStep(Credentials& credentials, XstsTarget target) noexcept;
		~XboxSecurityTokenStep() noexcept override = default;

		[[nodiscard]] QString description() const override;

	  public slots:
		void execute() override;

	  private slots:
		void onRequestCompleted();

	  private:
		[[nodiscard]] QString relyingParty() const;
		[[nodiscard]] QString targetName() const;
		[[nodiscard]] AuthToken& targetToken();
		bool handleStsError();

		XstsTarget m_target;

		std::shared_ptr<QByteArray> m_response;
		Net::Upload::Ptr m_request;
		NetJob::Ptr m_task;
	};

} // namespace projt::minecraft::auth
