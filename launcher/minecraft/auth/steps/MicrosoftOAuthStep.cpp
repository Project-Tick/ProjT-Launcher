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
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "MicrosoftOAuthStep.hpp"

#include <QAbstractOAuth2>
#include <QCoreApplication>
#include <QNetworkRequest>
#include <QOAuthHttpServerReplyHandler>
#include <QOAuthOobReplyHandler>
#include <QProcess>
#include <QSettings>
#include <QStandardPaths>

#include "Application.h"
#include "BuildConfig.h"

namespace projt::minecraft::auth
{

	namespace
	{

		/**
		 * Custom OOB reply handler that forwards OAuth callbacks from the application.
		 */
		class CustomSchemeReplyHandler : public QOAuthOobReplyHandler
		{
			Q_OBJECT

		  public:
			explicit CustomSchemeReplyHandler(QObject* parent = nullptr) : QOAuthOobReplyHandler(parent)
			{
				connect(APPLICATION, &Application::oauthReplyRecieved, this, &QOAuthOobReplyHandler::callbackReceived);
			}

			~CustomSchemeReplyHandler() override
			{
				disconnect(APPLICATION,
						   &Application::oauthReplyRecieved,
						   this,
						   &QOAuthOobReplyHandler::callbackReceived);
			}

			[[nodiscard]] QString callback() const override
			{
				return BuildConfig.LAUNCHER_APP_BINARY_NAME + QStringLiteral("://oauth/microsoft");
			}
		};

		/**
		 * Check if the custom URL scheme handler is registered with the OS.
		 */
		[[nodiscard]] bool isCustomSchemeRegistered()
		{
#ifdef Q_OS_LINUX
			QProcess process;
			process.start(QStringLiteral("xdg-mime"),
						  { QStringLiteral("query"),
							QStringLiteral("default"),
							QStringLiteral("x-scheme-handler/") + BuildConfig.LAUNCHER_APP_BINARY_NAME });
			process.waitForFinished();
			const QString output = process.readAllStandardOutput().trimmed();
			return output.contains(BuildConfig.LAUNCHER_APP_BINARY_NAME);
#elif defined(Q_OS_WIN)
			const QString regPath =
				QStringLiteral("HKEY_CURRENT_USER\\Software\\Classes\\%1").arg(BuildConfig.LAUNCHER_APP_BINARY_NAME);
			const QSettings settings(regPath, QSettings::NativeFormat);
			return settings.contains(QStringLiteral("shell/open/command/."));
#else
			return true;
#endif
		}

	} // namespace

	MicrosoftOAuthStep::MicrosoftOAuthStep(Credentials& credentials, bool silentRefresh) noexcept
		: Step(credentials),
		  m_silentRefresh(silentRefresh),
		  m_clientId(APPLICATION->getMSAClientID())
	{
		setupOAuthHandlers();
	}

	QString MicrosoftOAuthStep::description() const
	{
		return m_silentRefresh ? tr("Refreshing Microsoft account token.") : tr("Logging in with Microsoft account.");
	}

	void MicrosoftOAuthStep::setupOAuthHandlers()
	{
		// Choose appropriate reply handler based on environment
		if (shouldUseCustomScheme())
		{
			m_oauth.setReplyHandler(new CustomSchemeReplyHandler(this));
		}
		else
		{
			auto* httpHandler = new QOAuthHttpServerReplyHandler(this);
			httpHandler->setCallbackText(QStringLiteral(R"XXX(
            <noscript>
              <meta http-equiv="Refresh" content="0; URL=%1" />
            </noscript>
            Login Successful, redirecting...
            <script>
              window.location.replace("%1");
            </script>
            )XXX")
											 .arg(BuildConfig.LOGIN_CALLBACK_URL));
			m_oauth.setReplyHandler(httpHandler);
		}

		// Configure OAuth endpoints
		m_oauth.setAuthorizationUrl(
			QUrl(QStringLiteral("https://login.microsoftonline.com/consumers/oauth2/v2.0/authorize")));
		m_oauth.setAccessTokenUrl(
			QUrl(QStringLiteral("https://login.microsoftonline.com/consumers/oauth2/v2.0/token")));
		m_oauth.setScope(QStringLiteral("XboxLive.SignIn XboxLive.offline_access"));
		m_oauth.setClientIdentifier(m_clientId);
		m_oauth.setNetworkAccessManager(APPLICATION->network().get());

		// Connect signals
		connect(&m_oauth, &QOAuth2AuthorizationCodeFlow::granted, this, &MicrosoftOAuthStep::onGranted);
		connect(&m_oauth,
				&QOAuth2AuthorizationCodeFlow::authorizeWithBrowser,
				this,
				&MicrosoftOAuthStep::openBrowserRequested);
		connect(&m_oauth,
				&QOAuth2AuthorizationCodeFlow::authorizeWithBrowser,
				this,
				&MicrosoftOAuthStep::browserAuthRequired);
		connect(&m_oauth, &QOAuth2AuthorizationCodeFlow::requestFailed, this, &MicrosoftOAuthStep::onRequestFailed);
		connect(&m_oauth, &QOAuth2AuthorizationCodeFlow::error, this, &MicrosoftOAuthStep::onError);
		connect(&m_oauth,
				&QOAuth2AuthorizationCodeFlow::extraTokensChanged,
				this,
				&MicrosoftOAuthStep::onExtraTokensChanged);
		connect(&m_oauth,
				&QOAuth2AuthorizationCodeFlow::clientIdentifierChanged,
				this,
				&MicrosoftOAuthStep::onClientIdChanged);
	}

	bool MicrosoftOAuthStep::shouldUseCustomScheme() const
	{
		// Use HTTP server handler for AppImage, portable, or unregistered scheme
		const bool isAppImage = QCoreApplication::applicationFilePath().startsWith(QStringLiteral("/tmp/.mount_"));
		const bool isPortable = APPLICATION->isPortable();
		return !isAppImage && !isPortable && isCustomSchemeRegistered();
	}

	void MicrosoftOAuthStep::execute()
	{
		if (m_silentRefresh)
		{
			// Validate preconditions for silent refresh
			if (m_credentials.msaClientId != m_clientId)
			{
				emit completed(StepResult::Disabled, tr("Microsoft client ID has changed. Please log in again."));
				return;
			}

			if (!m_credentials.msaToken.hasRefreshToken())
			{
				emit completed(StepResult::Disabled, tr("No refresh token available. Please log in again."));
				return;
			}

			m_oauth.setRefreshToken(m_credentials.msaToken.refreshToken);
			m_oauth.refreshAccessToken();
		}
		else
		{
			// Interactive login - clear existing credentials
			m_credentials			  = Credentials{};
			m_credentials.msaClientId = m_clientId;

			// Force account selection prompt
			m_oauth.setModifyParametersFunction(
				[](QAbstractOAuth::Stage, QMultiMap<QString, QVariant>* params)
				{ params->insert(QStringLiteral("prompt"), QStringLiteral("select_account")); });

			m_oauth.grant();
		}
	}

	void MicrosoftOAuthStep::onGranted()
	{
		m_credentials.msaClientId			= m_oauth.clientIdentifier();
		m_credentials.msaToken.issuedAt		= QDateTime::currentDateTimeUtc();
		m_credentials.msaToken.expiresAt	= m_oauth.expirationAt();
		m_credentials.msaToken.metadata		= m_oauth.extraTokens();
		m_credentials.msaToken.refreshToken = m_oauth.refreshToken();
		m_credentials.msaToken.accessToken	= m_oauth.token();
		m_credentials.msaToken.validity		= TokenValidity::Certain;

		emit completed(StepResult::Continue, tr("Microsoft authentication successful."));
	}

	void MicrosoftOAuthStep::onRequestFailed(QAbstractOAuth2::Error err)
	{
		StepResult result = StepResult::HardFailure;

		if (m_oauth.status() == QAbstractOAuth::Status::Granted || m_silentRefresh)
		{
			result = (err == QAbstractOAuth2::Error::NetworkError) ? StepResult::Offline : StepResult::SoftFailure;
		}

		const QString message =
			m_silentRefresh ? tr("Failed to refresh Microsoft token.") : tr("Microsoft authentication failed.");
		qWarning() << message;
		emit completed(result, message);
	}

	void MicrosoftOAuthStep::onError(const QString& error, const QString& errorDescription, const QUrl& /*uri*/)
	{
		qWarning() << "OAuth error:" << error << "-" << errorDescription;
		emit completed(StepResult::HardFailure, errorDescription.isEmpty() ? error : errorDescription);
	}

	void MicrosoftOAuthStep::onExtraTokensChanged(const QVariantMap& tokens)
	{
		m_credentials.msaToken.metadata = tokens;
	}

	void MicrosoftOAuthStep::onClientIdChanged(const QString& clientId)
	{
		m_credentials.msaClientId = clientId;
	}

} // namespace projt::minecraft::auth

#include "MicrosoftOAuthStep.moc"
