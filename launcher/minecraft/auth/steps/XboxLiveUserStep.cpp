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

#include "XboxLiveUserStep.hpp"

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>

#include "Application.h"
#include "net/NetUtils.h"
#include "net/RawHeaderProxy.h"

namespace projt::minecraft::auth
{

	namespace
	{

		constexpr auto kXboxUserAuthUrl = "https://user.auth.xboxlive.com/user/authenticate";

		/**
		 * Parse Xbox token response.
		 * Returns true on success, false on parse error.
		 */
		[[nodiscard]] bool parseXboxTokenResponse(const QByteArray& data, AuthToken& token, const QString& tokenName)
		{
			QJsonParseError err;
			const auto doc = QJsonDocument::fromJson(data, &err);
			if (err.error != QJsonParseError::NoError || !doc.isObject())
			{
				qWarning() << "Failed to parse" << tokenName << "response:" << err.errorString();
				return false;
			}

			const auto obj = doc.object();

			// Parse issue and expiry times
			const QString issued  = obj.value(QStringLiteral("IssueInstant")).toString();
			const QString expires = obj.value(QStringLiteral("NotAfter")).toString();
			token.issuedAt		  = QDateTime::fromString(issued, Qt::ISODate);
			token.expiresAt		  = QDateTime::fromString(expires, Qt::ISODate);
			token.accessToken	  = obj.value(QStringLiteral("Token")).toString();

			// Parse display claims for user hash (uhs)
			const auto displayClaims = obj.value(QStringLiteral("DisplayClaims")).toObject();
			const auto xui			 = displayClaims.value(QStringLiteral("xui")).toArray();
			if (!xui.isEmpty())
			{
				const auto firstClaim = xui.first().toObject();
				token.metadata.insert(QStringLiteral("uhs"), firstClaim.value(QStringLiteral("uhs")).toString());
			}

			token.validity = TokenValidity::Certain;

			if (token.accessToken.isEmpty())
			{
				qWarning() << "Empty" << tokenName << "token received";
				return false;
			}

			return true;
		}

	} // namespace

	XboxLiveUserStep::XboxLiveUserStep(Credentials& credentials) noexcept : Step(credentials)
	{}

	QString XboxLiveUserStep::description() const
	{
		return tr("Authenticating with Xbox Live.");
	}

	void XboxLiveUserStep::execute()
	{
		const QString requestBody = QStringLiteral(R"({
        "Properties": {
            "AuthMethod": "RPS",
            "SiteName": "user.auth.xboxlive.com",
            "RpsTicket": "d=%1"
        },
        "RelyingParty": "http://auth.xboxlive.com",
        "TokenType": "JWT"
    })")
										.arg(m_credentials.msaToken.accessToken);

		const QUrl url(QString::fromLatin1(kXboxUserAuthUrl));
		const auto headers = QList<Net::HeaderPair>{ { "Content-Type", "application/json" },
													 { "Accept", "application/json" },
													 { "x-xbl-contract-version", "1" } };

		m_response = std::make_shared<QByteArray>();
		m_request  = Net::Upload::makeByteArray(url, m_response, requestBody.toUtf8());
		m_request->addHeaderProxy(new Net::RawHeaderProxy(headers));

		m_task = NetJob::Ptr::create(QStringLiteral("XboxLiveUserAuth"), APPLICATION->network());
		m_task->setAskRetry(false);
		m_task->addNetAction(m_request);

		connect(m_task.get(), &Task::finished, this, &XboxLiveUserStep::onRequestCompleted);
		m_task->start();

		qDebug() << "Authenticating with Xbox Live...";
	}

	void XboxLiveUserStep::onRequestCompleted()
	{
		if (m_request->error() != QNetworkReply::NoError)
		{
			qWarning() << "Xbox Live user auth error:" << m_request->error();

			const StepResult result =
				Net::isApplicationError(m_request->error()) ? StepResult::SoftFailure : StepResult::Offline;

			emit completed(result, tr("Xbox Live authentication failed: %1").arg(m_request->errorString()));
			return;
		}

		if (!parseXboxTokenResponse(*m_response, m_credentials.xboxUserToken, QStringLiteral("User")))
		{
			emit completed(StepResult::SoftFailure, tr("Could not parse Xbox Live user token response."));
			return;
		}

		emit completed(StepResult::Continue, tr("Got Xbox Live user token."));
	}

} // namespace projt::minecraft::auth
