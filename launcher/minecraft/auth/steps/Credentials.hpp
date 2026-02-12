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

#include <QByteArray>
#include <QDateTime>
#include <QMap>
#include <QString>
#include <QVariantMap>

namespace projt::minecraft::auth
{

	/**
	 * Validity state for tokens and data.
	 */
	enum class TokenValidity
	{
		None,	 ///< Not validated or expired
		Assumed, ///< Assumed valid (e.g., loaded from disk)
		Certain	 ///< Verified valid by server
	};

	/**
	 * Generic OAuth/authentication token.
	 */
	struct AuthToken
	{
		QDateTime issuedAt;	  ///< When the token was issued
		QDateTime expiresAt;  ///< When the token expires
		QString accessToken;  ///< The access token value
		QString refreshToken; ///< OAuth refresh token (if applicable)
		QVariantMap metadata; ///< Additional token data (e.g., user hash)

		TokenValidity validity = TokenValidity::None;
		bool persist		   = true; ///< Whether to save this token to disk

		[[nodiscard]] bool isExpired() const noexcept
		{
			return expiresAt.isValid() && QDateTime::currentDateTimeUtc() >= expiresAt;
		}

		[[nodiscard]] bool hasRefreshToken() const noexcept
		{
			return !refreshToken.isEmpty();
		}
	};

	/**
	 * Minecraft player skin data.
	 */
	struct PlayerSkin
	{
		QString id;
		QString url;
		QString variant; ///< "CLASSIC" or "SLIM"
		QByteArray imageData;

		[[nodiscard]] bool isEmpty() const noexcept
		{
			return id.isEmpty();
		}
	};

	/**
	 * Minecraft player cape data.
	 */
	struct PlayerCape
	{
		QString id;
		QString url;
		QString alias;
		QByteArray imageData;

		[[nodiscard]] bool isEmpty() const noexcept
		{
			return id.isEmpty();
		}
	};

	/**
	 * Minecraft game entitlements (ownership info).
	 */
	struct GameEntitlements
	{
		bool ownsMinecraft	   = false;
		bool canPlayMinecraft  = false;
		TokenValidity validity = TokenValidity::None;

		[[nodiscard]] bool isValid() const noexcept
		{
			return validity != TokenValidity::None;
		}
	};

	/**
	 * Minecraft Java Edition profile.
	 */
	struct MinecraftJavaProfile
	{
		QString id;	  ///< UUID without dashes
		QString name; ///< Player name (gamertag)
		PlayerSkin skin;
		QString activeCapeId;
		QMap<QString, PlayerCape> capes;
		TokenValidity validity = TokenValidity::None;

		[[nodiscard]] bool hasProfile() const noexcept
		{
			return !id.isEmpty();
		}
		[[nodiscard]] bool hasName() const noexcept
		{
			return !name.isEmpty();
		}
	};

	/**
	 * Account type enumeration.
	 */
	enum class AccountKind
	{
		Microsoft, ///< Microsoft/Xbox Live authenticated
		Offline	   ///< Offline mode (no authentication)
	};

	/**
	 * Account status enumeration.
	 */
	enum class AccountStatus
	{
		Unchecked, ///< Not yet validated
		Offline,   ///< Network unavailable
		Working,   ///< Auth in progress
		Online,	   ///< Fully authenticated
		Disabled,  ///< Disabled (e.g., client ID mismatch)
		Error,	   ///< Error state
		Expired,   ///< Tokens expired, needs refresh
		Gone	   ///< Account no longer exists
	};

	/**
	 * Complete authentication credentials for a Minecraft account.
	 *
	 * This structure holds all tokens and profile information needed to
	 * authenticate and play Minecraft. It is passed by reference to Step
	 * implementations which populate fields as authentication progresses.
	 */
	struct Credentials
	{
		// === Account identification ===
		AccountKind kind = AccountKind::Microsoft;
		QString internalId;	 ///< Internal account identifier
		QString msaClientId; ///< Microsoft Application client ID used

		// === Microsoft authentication chain ===
		AuthToken msaToken;				  ///< Microsoft OAuth token
		AuthToken xboxUserToken;		  ///< XBL user token
		AuthToken xboxServiceToken;		  ///< XSTS token for Xbox services
		AuthToken minecraftServicesToken; ///< XSTS token for Minecraft services

		// === Minecraft authentication ===
		AuthToken minecraftAccessToken; ///< Yggdrasil-style access token
		MinecraftJavaProfile profile;	///< Player profile
		GameEntitlements entitlements;	///< Game ownership

		// === Runtime state (not persisted) ===
		AccountStatus status = AccountStatus::Unchecked;
		QString lastError;

		// === Convenience accessors ===

		/**
		 * Display string for this account (gamertag or profile name).
		 */
		[[nodiscard]] QString displayName() const noexcept
		{
			return profile.hasName() ? profile.name : QStringLiteral("(unknown)");
		}

		/**
		 * Access token to pass to the game.
		 */
		[[nodiscard]] QString accessToken() const noexcept
		{
			return minecraftAccessToken.accessToken;
		}

		/**
		 * Profile UUID for game launch.
		 */
		[[nodiscard]] QString profileId() const noexcept
		{
			return profile.id;
		}

		/**
		 * Profile name for game launch.
		 */
		[[nodiscard]] QString profileName() const noexcept
		{
			return profile.name;
		}

		/**
		 * Xbox user hash (uhs) from token metadata.
		 */
		[[nodiscard]] QString xboxUserHash() const noexcept
		{
			return xboxUserToken.metadata.value(QStringLiteral("uhs")).toString();
		}
	};

} // namespace projt::minecraft::auth
