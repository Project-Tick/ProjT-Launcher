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

#include "Step.hpp"
#include "net/NetJob.h"
#include "net/Upload.h"

namespace projt::minecraft::auth
{

	/**
	 * Minecraft Services login step.
	 *
	 * Exchanges the XSTS token for a Minecraft access token (Yggdrasil-style).
	 * This token is used to authenticate with Minecraft game servers.
	 *
	 * Endpoint: https://api.minecraftservices.com/launcher/login
	 */
	class MinecraftServicesLoginStep : public Step
	{
		Q_OBJECT

	  public:
		explicit MinecraftServicesLoginStep(Credentials& credentials) noexcept;
		~MinecraftServicesLoginStep() noexcept override = default;

		[[nodiscard]] QString description() const override;

	  public slots:
		void execute() override;

	  private slots:
		void onRequestCompleted();

	  private:
		std::shared_ptr<QByteArray> m_response;
		Net::Upload::Ptr m_request;
		NetJob::Ptr m_task;
	};

} // namespace projt::minecraft::auth
