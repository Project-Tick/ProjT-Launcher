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
#include <QOAuth2AuthorizationCodeFlow>
#include <QString>

#include "Step.hpp"

namespace projt::minecraft::auth
{

	/**
	 * Microsoft OAuth 2.0 Authorization Code Flow step.
	 *
	 * Handles interactive browser-based login or silent token refresh using
	 * the standard OAuth 2.0 authorization code flow. Upon success, populates
	 * the MSA token in Credentials.
	 *
	 * This step supports two modes:
	 * - Interactive: Opens browser for user login
	 * - Silent: Attempts refresh using stored refresh token
	 */
	class MicrosoftOAuthStep : public Step
	{
		Q_OBJECT

	  public:
		/**
		 * Construct a new MSA OAuth step.
		 * @param credentials Credentials to populate with MSA token.
		 * @param silentRefresh If true, attempt silent refresh; if false, interactive login.
		 */
		explicit MicrosoftOAuthStep(Credentials& credentials, bool silentRefresh = false) noexcept;
		~MicrosoftOAuthStep() noexcept override = default;

		[[nodiscard]] QString description() const override;

	  public slots:
		void execute() override;

	  signals:
		/**
		 * Emitted when browser authorization is required (interactive mode).
		 * @param url URL to open in user's browser.
		 */
		void openBrowserRequested(const QUrl& url);

	  private slots:
		void onGranted();
		void onRequestFailed(QAbstractOAuth2::Error error);
		void onError(const QString& error, const QString& errorDescription, const QUrl& uri);
		void onExtraTokensChanged(const QVariantMap& tokens);
		void onClientIdChanged(const QString& clientId);

	  private:
		void setupOAuthHandlers();
		[[nodiscard]] bool shouldUseCustomScheme() const;

		bool m_silentRefresh = false;
		QString m_clientId;
		QOAuth2AuthorizationCodeFlow m_oauth;
	};

} // namespace projt::minecraft::auth
