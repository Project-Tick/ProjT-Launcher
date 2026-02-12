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
#include <QTimer>
#include <memory>

#include "Step.hpp"
#include "net/NetJob.h"
#include "net/Upload.h"

namespace projt::minecraft::auth
{

	/**
	 * Microsoft OAuth 2.0 Device Code Flow step.
	 *
	 * Used for environments where browser-based login is impractical.
	 * Displays a code for the user to enter at a Microsoft URL.
	 *
	 * Flow:
	 * 1. Request device code from Microsoft
	 * 2. Emit deviceCodeReady signal with code and URL
	 * 3. Poll for user completion
	 * 4. On success, populate MSA token in Credentials
	 */
	class DeviceCodeAuthStep : public Step
	{
		Q_OBJECT

	  public:
		explicit DeviceCodeAuthStep(Credentials& credentials) noexcept;
		~DeviceCodeAuthStep() noexcept override = default;

		[[nodiscard]] QString description() const override;

	  public slots:
		void execute() override;
		void cancel() noexcept override;

	  private slots:
		void onDeviceCodeReceived();
		void pollForCompletion();
		void onPollResponse();

	  private:
		void startPolling(int intervalSecs, int expiresInSecs);

		QString m_clientId;
		QString m_deviceCode;
		int m_pollInterval = 5; // seconds
		bool m_cancelled   = false;

		QTimer m_pollTimer;
		QTimer m_expirationTimer;

		std::shared_ptr<QByteArray> m_response;
		Net::Upload::Ptr m_request;
		NetJob::Ptr m_task;
	};

} // namespace projt::minecraft::auth
