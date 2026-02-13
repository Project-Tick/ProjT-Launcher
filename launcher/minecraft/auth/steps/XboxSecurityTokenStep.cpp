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

#include "XboxSecurityTokenStep.hpp"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QNetworkRequest>

#include "Application.h"
#include "minecraft/Logging.h"
#include "net/NetUtils.h"
#include "net/RawHeaderProxy.h"

namespace projt::minecraft::auth
{

	namespace
	{

		constexpr auto kXstsAuthorizeUrl = "https://xsts.auth.xboxlive.com/xsts/authorize";

		/**
		 * Parse Xbox token response.
		 */
		[[nodiscard]] bool parseXboxTokenResponse(const QByteArray& data, AuthToken& token)
		{
			QJsonParseError err;
			const auto doc = QJsonDocument::fromJson(data, &err);
			if (err.error != QJsonParseError::NoError || !doc.isObject())
			{
				qWarning() << "Failed to parse XSTS response:" << err.errorString();
				return false;
			}

			const auto obj = doc.object();

			token.issuedAt	= QDateTime::fromString(obj.value(QStringLiteral("IssueInstant")).toString(), Qt::ISODate);
			token.expiresAt = QDateTime::fromString(obj.value(QStringLiteral("NotAfter")).toString(), Qt::ISODate);
			token.accessToken = obj.value(QStringLiteral("Token")).toString();

			const auto displayClaims = obj.value(QStringLiteral("DisplayClaims")).toObject();
			const auto xui			 = displayClaims.value(QStringLiteral("xui")).toArray();
			if (!xui.isEmpty())
			{
				const auto firstClaim = xui.first().toObject();
				token.metadata.insert(QStringLiteral("uhs"), firstClaim.value(QStringLiteral("uhs")).toString());
			}

			token.validity = TokenValidity::Certain;
			return !token.accessToken.isEmpty();
		}

		/**
		 * XSTS error code messages.
		 * See: https://wiki.vg/Microsoft_Authentication_Scheme#Authenticate_with_XSTS
		 */
		struct XstsErrorInfo
		{
			int64_t code;
			const char* message;
		};

		constexpr std::array kXstsErrors = {
			XstsErrorInfo{ 2148916227, "This Microsoft account was banned by Xbox." },
			XstsErrorInfo{ 2148916229, "This account is restricted. Please check parental controls." },
			XstsErrorInfo{ 2148916233, "This account does not have an Xbox Live profile. Purchase the game first." },
			XstsErrorInfo{ 2148916234, "Please accept Xbox Terms of Service and try again." },
			XstsErrorInfo{ 2148916235, "Xbox Live is not available in your region." },
			XstsErrorInfo{ 2148916236, "This account requires age verification." },
			XstsErrorInfo{ 2148916237, "This account has reached its playtime limit." },
			XstsErrorInfo{ 2148916238, "This account is underaged and not linked to a family." }
		};

	} // namespace

	XboxSecurityTokenStep::XboxSecurityTokenStep(Credentials& credentials, XstsTarget target) noexcept
		: Step(credentials),
		  m_target(target)
	{}

	QString XboxSecurityTokenStep::description() const
	{
		return tr("Getting authorization for %1 services.").arg(targetName());
	}

	QString XboxSecurityTokenStep::relyingParty() const
	{
		switch (m_target)
		{
			case XstsTarget::XboxLive: return QStringLiteral("http://xboxlive.com");
			case XstsTarget::MinecraftServices: return QStringLiteral("rp://api.minecraftservices.com/");
		}
		Q_UNREACHABLE();
	}

	QString XboxSecurityTokenStep::targetName() const
	{
		switch (m_target)
		{
			case XstsTarget::XboxLive: return QStringLiteral("Xbox Live");
			case XstsTarget::MinecraftServices: return QStringLiteral("Minecraft");
		}
		Q_UNREACHABLE();
	}

	AuthToken& XboxSecurityTokenStep::targetToken()
	{
		switch (m_target)
		{
			case XstsTarget::XboxLive: return m_credentials.xboxServiceToken;
			case XstsTarget::MinecraftServices: return m_credentials.minecraftServicesToken;
		}
		Q_UNREACHABLE();
	}

	void XboxSecurityTokenStep::execute()
	{
		const QString requestBody = QStringLiteral(R"({
        "Properties": {
            "SandboxId": "RETAIL",
            "UserTokens": ["%1"]
        },
        "RelyingParty": "%2",
        "TokenType": "JWT"
    })")
										.arg(m_credentials.xboxUserToken.accessToken, relyingParty());

		const QUrl url(QString::fromLatin1(kXstsAuthorizeUrl));
		const auto headers =
			QList<Net::HeaderPair>{ { "Content-Type", "application/json" }, { "Accept", "application/json" } };

		m_response = std::make_shared<QByteArray>();
		m_request  = Net::Upload::makeByteArray(url, m_response, requestBody.toUtf8());
		m_request->addHeaderProxy(new Net::RawHeaderProxy(headers));

		m_task = NetJob::Ptr::create(QStringLiteral("XstsAuthorize"), APPLICATION->network());
		m_task->setAskRetry(false);
		m_task->addNetAction(m_request);

		connect(m_task.get(), &Task::finished, this, &XboxSecurityTokenStep::onRequestCompleted);
		m_task->start();

		qDebug() << "Getting XSTS token for" << relyingParty();
	}

	void XboxSecurityTokenStep::onRequestCompleted()
	{
		qCDebug(authCredentials()) << *m_response;

		if (m_request->error() != QNetworkReply::NoError)
		{
			qWarning() << "XSTS request error:" << m_request->error();

			if (Net::isApplicationError(m_request->error()))
			{
				if (handleStsError())
				{
					return;
				}
				emit completed(StepResult::SoftFailure,
							   tr("Failed to get %1 authorization: %2").arg(targetName(), m_request->errorString()));
			}
			else
			{
				emit completed(StepResult::Offline,
							   tr("Failed to get %1 authorization: %2").arg(targetName(), m_request->errorString()));
			}
			return;
		}

		AuthToken token;
		if (!parseXboxTokenResponse(*m_response, token))
		{
			emit completed(StepResult::SoftFailure, tr("Could not parse %1 authorization response.").arg(targetName()));
			return;
		}

		// Verify user hash matches
		const QString responseUhs = token.metadata.value(QStringLiteral("uhs")).toString();
		if (responseUhs != m_credentials.xboxUserHash())
		{
			emit completed(StepResult::SoftFailure, tr("User hash mismatch in %1 authorization.").arg(targetName()));
			return;
		}

		targetToken() = token;
		emit completed(StepResult::Continue, tr("Got %1 authorization.").arg(targetName()));
	}

	bool XboxSecurityTokenStep::handleStsError()
	{
		if (m_request->error() != QNetworkReply::AuthenticationRequiredError)
		{
			return false;
		}

		QJsonParseError jsonError;
		const auto doc = QJsonDocument::fromJson(*m_response, &jsonError);
		if (jsonError.error != QJsonParseError::NoError)
		{
			emit completed(StepResult::SoftFailure,
						   tr("Cannot parse XSTS error response: %1").arg(jsonError.errorString()));
			return true;
		}

		const auto obj		 = doc.object();
		const auto errorCode = static_cast<int64_t>(obj.value(QStringLiteral("XErr")).toDouble());

		if (errorCode == 0)
		{
			emit completed(StepResult::SoftFailure, tr("XSTS error response missing error code."));
			return true;
		}

		// Look up error message
		for (const auto& [code, message] : kXstsErrors)
		{
			if (code == errorCode)
			{
				emit completed(StepResult::SoftFailure, tr(message));
				return true;
			}
		}

		emit completed(StepResult::SoftFailure, tr("Unknown XSTS error: %1").arg(errorCode));
		return true;
	}

} // namespace projt::minecraft::auth
