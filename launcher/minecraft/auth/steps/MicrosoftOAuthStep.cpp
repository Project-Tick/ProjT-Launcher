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

#include "MicrosoftOAuthStep.hpp"

#include <QAbstractOAuth2>
#include <QCoreApplication>
#include <QFileInfo>
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
		 * Check if the custom URL scheme handler is registered with the OS AND
		 * that the registered handler points to the currently running binary.
		 *
		 * This is important when multiple builds of the launcher coexist on the same
		 * machine (e.g. an installed release and a locally compiled dev build).
		 * If the registered handler points to a *different* binary, the OAuth callback
		 * URL would be intercepted by that other instance instead of the current one,
		 * causing the login flow to fail silently. In that case we fall back to the
		 * HTTP loopback server handler which is always self-contained.
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
			if (!output.contains(BuildConfig.LAUNCHER_APP_BINARY_NAME))
				return false;

			// Also verify the registered .desktop entry resolves to our own binary.
			// xdg-mime returns something like "projtlauncher.desktop"; locate it and
			// read the Exec= line to compare against our own executable path.
			const QString desktopFileName = output.section(QLatin1Char('\n'), 0, 0).trimmed();
			const QStringList dataDirs	  = QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);
			for (const QString& dataDir : dataDirs)
			{
				const QString desktopPath = dataDir + QStringLiteral("/applications/") + desktopFileName;
				QSettings desktopFile(desktopPath, QSettings::IniFormat);
				desktopFile.beginGroup(QStringLiteral("Desktop Entry"));
				const QString execLine = desktopFile.value(QStringLiteral("Exec")).toString();
				desktopFile.endGroup();
				if (execLine.isEmpty())
					continue;
				// Exec= may contain %U or similar; take only the binary part.
				const QString registeredBin = execLine.section(QLatin1Char(' '), 0, 0);
				const QFileInfo currentBin(QCoreApplication::applicationFilePath());
				const QFileInfo registeredBinInfo(registeredBin);
				if (registeredBinInfo.canonicalFilePath() == currentBin.canonicalFilePath())
					return true;
				// Registered handler is a different binary → do not use custom scheme.
				qDebug() << "Custom URL scheme is registered for a different binary (" << registeredBin
						 << ") — falling back to HTTP loopback handler.";
				return false;
			}
			return true; // Could not verify; assume it's ours.
#elif defined(Q_OS_WIN)
			const QString regPath =
				QStringLiteral("HKEY_CURRENT_USER\\Software\\Classes\\%1").arg(BuildConfig.LAUNCHER_APP_BINARY_NAME);
			const QSettings settings(regPath, QSettings::NativeFormat);
			if (!settings.contains(QStringLiteral("shell/open/command/.")))
				return false;

			// Verify that the registered command actually points to this binary.
			// The registry value looks like: "C:\path\to\launcher.exe" "%1"
			QString registeredCmd = settings.value(QStringLiteral("shell/open/command/.")).toString();
			// Strip surrounding quotes from the executable portion.
			if (registeredCmd.startsWith(QLatin1Char('"')))
			{
				registeredCmd		 = registeredCmd.mid(1);
				const int closeQuote = registeredCmd.indexOf(QLatin1Char('"'));
				if (closeQuote >= 0)
					registeredCmd = registeredCmd.left(closeQuote);
			}
			else
			{
				// No quotes — executable ends at the first space.
				const int spaceIdx = registeredCmd.indexOf(QLatin1Char(' '));
				if (spaceIdx >= 0)
					registeredCmd = registeredCmd.left(spaceIdx);
			}

			const QFileInfo currentBin(QCoreApplication::applicationFilePath());
			const QFileInfo registeredBin(registeredCmd);
			if (registeredBin.canonicalFilePath().compare(currentBin.canonicalFilePath(), Qt::CaseInsensitive) == 0)
				return true;

			// The URL scheme is registered, but for a different launcher binary.
			// Fall back to the HTTP loopback handler so our OAuth callback reaches us.
			qDebug() << "Custom URL scheme is registered for a different binary (" << registeredCmd
					 << ") — falling back to HTTP loopback handler.";
			return false;
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
