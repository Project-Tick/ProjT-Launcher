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

#include "DeviceCodeAuthStep.hpp"

#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QUrlQuery>

#include "Application.h"
#include "Json.h"
#include "net/RawHeaderProxy.h"

namespace projt::minecraft::auth
{

	namespace
	{

		// Device authorization endpoints
		constexpr auto kDeviceCodeUrl = "https://login.microsoftonline.com/consumers/oauth2/v2.0/devicecode";
		constexpr auto kTokenUrl	  = "https://login.microsoftonline.com/consumers/oauth2/v2.0/token";

		/**
		 * Parse device code response from Microsoft.
		 */
		struct DeviceCodeResponse
		{
			QString deviceCode;
			QString userCode;
			QString verificationUri;
			int expiresIn = 0;
			int interval  = 5;
			QString error;
			QString errorDescription;

			[[nodiscard]] bool isValid() const noexcept
			{
				return !deviceCode.isEmpty() && !userCode.isEmpty() && !verificationUri.isEmpty() && expiresIn > 0;
			}
		};

		[[nodiscard]] DeviceCodeResponse parseDeviceCodeResponse(const QByteArray& data)
		{
			QJsonParseError err;
			const auto doc = QJsonDocument::fromJson(data, &err);
			if (err.error != QJsonParseError::NoError || !doc.isObject())
			{
				qWarning() << "Failed to parse device code response:" << err.errorString();
				return {};
			}

			const auto obj = doc.object();
			return { Json::ensureString(obj, "device_code"),	  Json::ensureString(obj, "user_code"),
					 Json::ensureString(obj, "verification_uri"), Json::ensureInteger(obj, "expires_in"),
					 Json::ensureInteger(obj, "interval", 5),	  Json::ensureString(obj, "error"),
					 Json::ensureString(obj, "error_description") };
		}

		/**
		 * Parse token response from Microsoft.
		 */
		struct TokenResponse
		{
			QString accessToken;
			QString tokenType;
			QString refreshToken;
			int expiresIn = 0;
			QString error;
			QString errorDescription;
			QVariantMap metadata;

			[[nodiscard]] bool isSuccess() const noexcept
			{
				return !accessToken.isEmpty();
			}
			[[nodiscard]] bool isPending() const noexcept
			{
				return error == QStringLiteral("authorization_pending");
			}
			[[nodiscard]] bool needsSlowDown() const noexcept
			{
				return error == QStringLiteral("slow_down");
			}
		};

		[[nodiscard]] TokenResponse parseTokenResponse(const QByteArray& data)
		{
			QJsonParseError err;
			const auto doc = QJsonDocument::fromJson(data, &err);
			if (err.error != QJsonParseError::NoError || !doc.isObject())
			{
				qWarning() << "Failed to parse token response:" << err.errorString();
				return {};
			}

			const auto obj = doc.object();
			return { Json::ensureString(obj, "access_token"),
					 Json::ensureString(obj, "token_type"),
					 Json::ensureString(obj, "refresh_token"),
					 Json::ensureInteger(obj, "expires_in"),
					 Json::ensureString(obj, "error"),
					 Json::ensureString(obj, "error_description"),
					 obj.toVariantMap() };
		}

	} // namespace

	DeviceCodeAuthStep::DeviceCodeAuthStep(Credentials& credentials) noexcept
		: Step(credentials),
		  m_clientId(APPLICATION->getMSAClientID())
	{
		m_pollTimer.setTimerType(Qt::VeryCoarseTimer);
		m_pollTimer.setSingleShot(true);
		m_expirationTimer.setTimerType(Qt::VeryCoarseTimer);
		m_expirationTimer.setSingleShot(true);

		connect(&m_expirationTimer, &QTimer::timeout, this, &DeviceCodeAuthStep::cancel);
		connect(&m_pollTimer, &QTimer::timeout, this, &DeviceCodeAuthStep::pollForCompletion);
	}

	QString DeviceCodeAuthStep::description() const
	{
		return tr("Logging in with Microsoft account (device code).");
	}

	void DeviceCodeAuthStep::execute()
	{
		QUrlQuery query;
		query.addQueryItem(QStringLiteral("client_id"), m_clientId);
		query.addQueryItem(QStringLiteral("scope"), QStringLiteral("XboxLive.SignIn XboxLive.offline_access"));

		const auto payload = query.query(QUrl::FullyEncoded).toUtf8();
		const QUrl url(QString::fromLatin1(kDeviceCodeUrl));

		const auto headers = QList<Net::HeaderPair>{ { "Content-Type", "application/x-www-form-urlencoded" },
													 { "Accept", "application/json" } };

		m_response = std::make_shared<QByteArray>();
		m_request  = Net::Upload::makeByteArray(url, m_response, payload);
		m_request->addHeaderProxy(new Net::RawHeaderProxy(headers));

		m_task = NetJob::Ptr::create(QStringLiteral("DeviceCodeRequest"), APPLICATION->network());
		m_task->setAskRetry(false);
		m_task->addNetAction(m_request);

		connect(m_task.get(), &Task::finished, this, &DeviceCodeAuthStep::onDeviceCodeReceived);
		m_task->start();
	}

	void DeviceCodeAuthStep::cancel() noexcept
	{
		m_cancelled = true;
		m_expirationTimer.stop();
		m_pollTimer.stop();

		if (m_request)
		{
			m_request->abort();
		}

		emit completed(StepResult::HardFailure, tr("Authentication cancelled or timed out."));
	}

	void DeviceCodeAuthStep::onDeviceCodeReceived()
	{
		const auto rsp = parseDeviceCodeResponse(*m_response);

		if (!rsp.error.isEmpty())
		{
			const QString msg = rsp.errorDescription.isEmpty() ? rsp.error : rsp.errorDescription;
			emit completed(StepResult::HardFailure, tr("Device authorization failed: %1").arg(msg));
			return;
		}

		if (!m_request->wasSuccessful() || m_request->error() != QNetworkReply::NoError)
		{
			emit completed(StepResult::HardFailure, tr("Failed to request device authorization."));
			return;
		}

		if (!rsp.isValid())
		{
			emit completed(StepResult::HardFailure, tr("Invalid device authorization response."));
			return;
		}

		m_deviceCode   = rsp.deviceCode;
		m_pollInterval = rsp.interval > 0 ? rsp.interval : 5;

		// Notify UI to display code
		emit deviceCodeReady(rsp.verificationUri, rsp.userCode, rsp.expiresIn);

		// Start polling
		startPolling(m_pollInterval, rsp.expiresIn);
	}

	void DeviceCodeAuthStep::startPolling(int intervalSecs, int expiresInSecs)
	{
		if (m_cancelled)
		{
			return;
		}

		m_expirationTimer.setInterval(expiresInSecs * 1000);
		m_expirationTimer.start();

		m_pollTimer.setInterval(intervalSecs * 1000);
		m_pollTimer.start();
	}

	void DeviceCodeAuthStep::pollForCompletion()
	{
		if (m_cancelled)
		{
			return;
		}

		QUrlQuery query;
		query.addQueryItem(QStringLiteral("client_id"), m_clientId);
		query.addQueryItem(QStringLiteral("grant_type"),
						   QStringLiteral("urn:ietf:params:oauth:grant-type:device_code"));
		query.addQueryItem(QStringLiteral("device_code"), m_deviceCode);

		const auto payload = query.query(QUrl::FullyEncoded).toUtf8();
		const QUrl url(QString::fromLatin1(kTokenUrl));

		const auto headers = QList<Net::HeaderPair>{ { "Content-Type", "application/x-www-form-urlencoded" },
													 { "Accept", "application/json" } };

		m_response = std::make_shared<QByteArray>();
		m_request  = Net::Upload::makeByteArray(url, m_response, payload);
		m_request->addHeaderProxy(new Net::RawHeaderProxy(headers));
		m_request->setNetwork(APPLICATION->network());

		connect(m_request.get(), &Task::finished, this, &DeviceCodeAuthStep::onPollResponse);
		m_request->start();
	}

	void DeviceCodeAuthStep::onPollResponse()
	{
		if (m_cancelled)
		{
			return;
		}

		// Handle timeout - exponential backoff per RFC 8628
		if (m_request->error() == QNetworkReply::TimeoutError)
		{
			m_pollInterval *= 2;
			m_pollTimer.setInterval(m_pollInterval * 1000);
			m_pollTimer.start();
			return;
		}

		const auto rsp = parseTokenResponse(*m_response);

		// Handle slow_down - increase interval by 5 seconds per RFC 8628
		if (rsp.needsSlowDown())
		{
			m_pollInterval += 5;
			m_pollTimer.setInterval(m_pollInterval * 1000);
			m_pollTimer.start();
			return;
		}

		// Authorization still pending - keep polling
		if (rsp.isPending())
		{
			m_pollTimer.start();
			return;
		}

		// Check for other errors
		if (!rsp.error.isEmpty())
		{
			const QString msg = rsp.errorDescription.isEmpty() ? rsp.error : rsp.errorDescription;
			emit completed(StepResult::HardFailure, tr("Device authentication failed: %1").arg(msg));
			return;
		}

		// Network error - retry
		if (!m_request->wasSuccessful() || m_request->error() != QNetworkReply::NoError)
		{
			m_pollTimer.start();
			return;
		}

		// Success!
		m_expirationTimer.stop();

		m_credentials.msaClientId			= m_clientId;
		m_credentials.msaToken.issuedAt		= QDateTime::currentDateTimeUtc();
		m_credentials.msaToken.expiresAt	= QDateTime::currentDateTimeUtc().addSecs(rsp.expiresIn);
		m_credentials.msaToken.metadata		= rsp.metadata;
		m_credentials.msaToken.refreshToken = rsp.refreshToken;
		m_credentials.msaToken.accessToken	= rsp.accessToken;
		m_credentials.msaToken.validity		= TokenValidity::Certain;

		emit completed(StepResult::Continue, tr("Microsoft authentication successful."));
	}

} // namespace projt::minecraft::auth
