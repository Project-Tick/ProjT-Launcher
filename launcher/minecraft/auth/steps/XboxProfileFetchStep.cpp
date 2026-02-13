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

#include "XboxProfileFetchStep.hpp"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QUrlQuery>

#include "Application.h"
#include "minecraft/Logging.h"
#include "net/NetUtils.h"
#include "net/RawHeaderProxy.h"

namespace projt::minecraft::auth
{

	namespace
	{

		constexpr auto kXboxProfileUrl = "https://profile.xboxlive.com/users/me/profile/settings";

		// Profile settings to request
		constexpr auto kProfileSettings = "GameDisplayName,AppDisplayName,AppDisplayPicRaw,GameDisplayPicRaw,"
										  "PublicGamerpic,ShowUserAsAvatar,Gamerscore,Gamertag,ModernGamertag,"
										  "ModernGamertagSuffix,UniqueModernGamertag,AccountTier,TenureLevel,"
										  "XboxOneRep,PreferredColor,Location,Bio,Watermarks,RealName,"
										  "RealNameOverride,IsQuarantined";

	} // namespace

	XboxProfileFetchStep::XboxProfileFetchStep(Credentials& credentials) noexcept : Step(credentials)
	{}

	QString XboxProfileFetchStep::description() const
	{
		return tr("Fetching Xbox profile.");
	}

	void XboxProfileFetchStep::execute()
	{
		QUrl url(QString::fromLatin1(kXboxProfileUrl));
		QUrlQuery query;
		query.addQueryItem(QStringLiteral("settings"), QString::fromLatin1(kProfileSettings));
		url.setQuery(query);

		const QString authHeader = QStringLiteral("XBL3.0 x=%1;%2")
									   .arg(m_credentials.xboxUserHash(), m_credentials.xboxServiceToken.accessToken);

		const auto headers = QList<Net::HeaderPair>{ { "Content-Type", "application/json" },
													 { "Accept", "application/json" },
													 { "x-xbl-contract-version", "3" },
													 { "Authorization", authHeader.toUtf8() } };

		m_response = std::make_shared<QByteArray>();
		m_request  = Net::Download::makeByteArray(url, m_response);
		m_request->addHeaderProxy(new Net::RawHeaderProxy(headers));

		m_task = NetJob::Ptr::create(QStringLiteral("XboxProfileFetch"), APPLICATION->network());
		m_task->setAskRetry(false);
		m_task->addNetAction(m_request);

		connect(m_task.get(), &Task::finished, this, &XboxProfileFetchStep::onRequestCompleted);
		m_task->start();

		qDebug() << "Fetching Xbox profile...";
	}

	void XboxProfileFetchStep::onRequestCompleted()
	{
		if (m_request->error() != QNetworkReply::NoError)
		{
			qWarning() << "Xbox profile fetch error:" << m_request->error();
			qCDebug(authCredentials()) << *m_response;

			// Profile fetch is optional - continue even on failure
			const StepResult result =
				Net::isApplicationError(m_request->error()) ? StepResult::SoftFailure : StepResult::Offline;

			emit completed(result, tr("Failed to fetch Xbox profile: %1").arg(m_request->errorString()));
			return;
		}

		qCDebug(authCredentials()) << "Xbox profile:" << *m_response;

		// Parse the response to extract gamertag
		parseProfileResponse();

		emit completed(StepResult::Continue, tr("Got Xbox profile."));
	}

	void XboxProfileFetchStep::parseProfileResponse()
	{
		QJsonParseError parseError;
		const auto doc = QJsonDocument::fromJson(*m_response, &parseError);

		if (parseError.error != QJsonParseError::NoError || !doc.isObject())
		{
			qWarning() << "Failed to parse Xbox profile response:" << parseError.errorString();
			return;
		}

		const auto root			= doc.object();
		const auto profileUsers = root.value(QStringLiteral("profileUsers")).toArray();

		if (profileUsers.isEmpty())
		{
			qWarning() << "No profile users in Xbox response";
			return;
		}

		const auto user		= profileUsers.first().toObject();
		const auto settings = user.value(QStringLiteral("settings")).toArray();

		for (const auto& settingValue : settings)
		{
			const auto setting = settingValue.toObject();
			const auto id	   = setting.value(QStringLiteral("id")).toString();
			const auto value   = setting.value(QStringLiteral("value")).toString();

			if (id == QStringLiteral("Gamertag"))
			{
				// Store gamertag in xboxServiceToken.metadata for legacy sync
				// accountDisplayString() expects this in xboxApiToken.extra["gtg"]
				m_credentials.xboxServiceToken.metadata[QStringLiteral("gtg")] = value;
				qDebug() << "Got Xbox gamertag:" << value;
			}
			else if (id == QStringLiteral("GameDisplayPicRaw"))
			{
				m_credentials.xboxServiceToken.metadata[QStringLiteral("gamerPicUrl")] = value;
			}
		}
	}

} // namespace projt::minecraft::auth
