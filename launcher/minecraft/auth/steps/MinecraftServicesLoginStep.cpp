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

#include "MinecraftServicesLoginStep.hpp"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QNetworkRequest>
#include <QUrl>

#include "Application.h"
#include "minecraft/Logging.h"
#include "net/NetUtils.h"
#include "net/RawHeaderProxy.h"

namespace projt::minecraft::auth
{

	namespace
	{

		constexpr auto kMinecraftLoginUrl = "https://api.minecraftservices.com/launcher/login";

		/**
		 * Parse Minecraft services authentication response.
		 */
		[[nodiscard]] bool parseMinecraftAuthResponse(const QByteArray& data, AuthToken& token)
		{
			QJsonParseError err;
			const auto doc = QJsonDocument::fromJson(data, &err);
			if (err.error != QJsonParseError::NoError || !doc.isObject())
			{
				qWarning() << "Failed to parse Minecraft login response:" << err.errorString();
				return false;
			}

			const auto obj = doc.object();

			token.accessToken = obj.value(QStringLiteral("access_token")).toString();
			token.issuedAt	  = QDateTime::currentDateTimeUtc();

			const int expiresIn = obj.value(QStringLiteral("expires_in")).toInt();
			if (expiresIn > 0)
			{
				token.expiresAt = token.issuedAt.addSecs(expiresIn);
			}

			// Store token type and other metadata
			token.metadata.insert(QStringLiteral("token_type"), obj.value(QStringLiteral("token_type")).toString());

			token.validity = TokenValidity::Certain;
			return !token.accessToken.isEmpty();
		}

	} // namespace

	MinecraftServicesLoginStep::MinecraftServicesLoginStep(Credentials& credentials) noexcept : Step(credentials)
	{}

	QString MinecraftServicesLoginStep::description() const
	{
		return tr("Logging in to Minecraft services.");
	}

	void MinecraftServicesLoginStep::execute()
	{
		const QString uhs	 = m_credentials.minecraftServicesToken.metadata.value(QStringLiteral("uhs")).toString();
		const QString xToken = m_credentials.minecraftServicesToken.accessToken;

		const QString requestBody = QStringLiteral(R"({
        "xtoken": "XBL3.0 x=%1;%2",
        "platform": "PC_LAUNCHER"
    })")
										.arg(uhs, xToken);

		const QUrl url(QString::fromLatin1(kMinecraftLoginUrl));
		const auto headers =
			QList<Net::HeaderPair>{ { "Content-Type", "application/json" }, { "Accept", "application/json" } };

		m_response = std::make_shared<QByteArray>();
		m_request  = Net::Upload::makeByteArray(url, m_response, requestBody.toUtf8());
		m_request->addHeaderProxy(new Net::RawHeaderProxy(headers));

		m_task = NetJob::Ptr::create(QStringLiteral("MinecraftServicesLogin"), APPLICATION->network());
		m_task->setAskRetry(false);
		m_task->addNetAction(m_request);

		connect(m_task.get(), &Task::finished, this, &MinecraftServicesLoginStep::onRequestCompleted);
		m_task->start();

		qDebug() << "Getting Minecraft access token...";
	}

	void MinecraftServicesLoginStep::onRequestCompleted()
	{
		qCDebug(authCredentials()) << *m_response;

		if (m_request->error() != QNetworkReply::NoError)
		{
			qWarning() << "Minecraft login error:" << m_request->error();

			const StepResult result =
				Net::isApplicationError(m_request->error()) ? StepResult::SoftFailure : StepResult::Offline;

			emit completed(result, tr("Failed to get Minecraft access token: %1").arg(m_request->errorString()));
			return;
		}

		if (!parseMinecraftAuthResponse(*m_response, m_credentials.minecraftAccessToken))
		{
			qWarning() << "Could not parse Minecraft login response";
			emit completed(StepResult::SoftFailure, tr("Failed to parse Minecraft access token response."));
			return;
		}

		emit completed(StepResult::Continue, tr("Got Minecraft access token."));
	}

} // namespace projt::minecraft::auth
