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

#include "MinecraftProfileFetchStep.hpp"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QNetworkRequest>

#include "Application.h"
#include "net/NetUtils.h"
#include "net/RawHeaderProxy.h"

namespace projt::minecraft::auth
{

	namespace
	{

		constexpr auto kMinecraftProfileUrl = "https://api.minecraftservices.com/minecraft/profile";

	} // namespace

	MinecraftProfileFetchStep::MinecraftProfileFetchStep(Credentials& credentials) noexcept : Step(credentials)
	{}

	QString MinecraftProfileFetchStep::description() const
	{
		return tr("Fetching Minecraft profile.");
	}

	void MinecraftProfileFetchStep::execute()
	{
		const QUrl url(QString::fromLatin1(kMinecraftProfileUrl));
		const QString authHeader = QStringLiteral("Bearer %1").arg(m_credentials.minecraftAccessToken.accessToken);

		const auto headers = QList<Net::HeaderPair>{ { "Content-Type", "application/json" },
													 { "Accept", "application/json" },
													 { "Authorization", authHeader.toUtf8() } };

		m_response = std::make_shared<QByteArray>();
		m_request  = Net::Download::makeByteArray(url, m_response);
		m_request->addHeaderProxy(new Net::RawHeaderProxy(headers));

		m_task = NetJob::Ptr::create(QStringLiteral("MinecraftProfileFetch"), APPLICATION->network());
		m_task->setAskRetry(false);
		m_task->addNetAction(m_request);

		connect(m_task.get(), &Task::finished, this, &MinecraftProfileFetchStep::onRequestCompleted);
		m_task->start();
	}

	void MinecraftProfileFetchStep::onRequestCompleted()
	{
		// 404 = no profile exists (valid state for new accounts)
		if (m_request->error() == QNetworkReply::ContentNotFoundError)
		{
			m_credentials.profile = MinecraftJavaProfile{};
			emit completed(StepResult::Continue, tr("Account has no Minecraft profile."));
			return;
		}

		if (m_request->error() != QNetworkReply::NoError)
		{
			qWarning() << "Minecraft profile fetch error:";
			qWarning() << "  HTTP Status:" << m_request->replyStatusCode();
			qWarning() << "  Error:" << m_request->error() << m_request->errorString();
			qWarning() << "  Response:" << QString::fromUtf8(*m_response);

			const StepResult result =
				Net::isApplicationError(m_request->error()) ? StepResult::SoftFailure : StepResult::Offline;

			emit completed(result, tr("Failed to fetch Minecraft profile: %1").arg(m_request->errorString()));
			return;
		}

		if (!parseProfileResponse(*m_response))
		{
			m_credentials.profile = MinecraftJavaProfile{};
			emit completed(StepResult::SoftFailure, tr("Could not parse Minecraft profile response."));
			return;
		}

		emit completed(StepResult::Continue, tr("Got Minecraft profile."));
	}

	bool MinecraftProfileFetchStep::parseProfileResponse(const QByteArray& data)
	{
		QJsonParseError err;
		const auto doc = QJsonDocument::fromJson(data, &err);
		if (err.error != QJsonParseError::NoError || !doc.isObject())
		{
			qWarning() << "Failed to parse Minecraft profile:" << err.errorString();
			return false;
		}

		const auto obj = doc.object();

		// Basic profile info
		m_credentials.profile.id   = obj.value(QStringLiteral("id")).toString();
		m_credentials.profile.name = obj.value(QStringLiteral("name")).toString();

		if (m_credentials.profile.id.isEmpty())
		{
			return false;
		}

		// Parse skins
		const auto skins = obj.value(QStringLiteral("skins")).toArray();
		for (const auto& skinVal : skins)
		{
			const auto skinObj	= skinVal.toObject();
			const QString state = skinObj.value(QStringLiteral("state")).toString();

			if (state == QStringLiteral("ACTIVE"))
			{
				m_credentials.profile.skin.id	   = skinObj.value(QStringLiteral("id")).toString();
				m_credentials.profile.skin.url	   = skinObj.value(QStringLiteral("url")).toString();
				m_credentials.profile.skin.variant = skinObj.value(QStringLiteral("variant")).toString();
				break;
			}
		}

		// Parse capes
		const auto capes = obj.value(QStringLiteral("capes")).toArray();
		for (const auto& capeVal : capes)
		{
			const auto capeObj	 = capeVal.toObject();
			const QString capeId = capeObj.value(QStringLiteral("id")).toString();

			PlayerCape cape;
			cape.id	   = capeId;
			cape.url   = capeObj.value(QStringLiteral("url")).toString();
			cape.alias = capeObj.value(QStringLiteral("alias")).toString();

			m_credentials.profile.capes.insert(capeId, cape);

			// Track active cape
			const QString state = capeObj.value(QStringLiteral("state")).toString();
			if (state == QStringLiteral("ACTIVE"))
			{
				m_credentials.profile.activeCapeId = capeId;
			}
		}

		m_credentials.profile.validity = TokenValidity::Certain;
		return true;
	}

} // namespace projt::minecraft::auth
