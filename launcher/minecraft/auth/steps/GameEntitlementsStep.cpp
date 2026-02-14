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

#include "GameEntitlementsStep.hpp"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QNetworkRequest>
#include <QUrl>
#include <QUuid>

#include "Application.h"
#include "minecraft/Logging.h"
#include "net/RawHeaderProxy.h"

namespace projt::minecraft::auth
{

	namespace
	{

		constexpr auto kEntitlementsUrl = "https://api.minecraftservices.com/entitlements/license";

	} // namespace

	GameEntitlementsStep::GameEntitlementsStep(Credentials& credentials) noexcept : Step(credentials)
	{}

	QString GameEntitlementsStep::description() const
	{
		return tr("Checking game ownership.");
	}

	void GameEntitlementsStep::execute()
	{
		// Generate unique request ID for validation
		m_requestId = QUuid::createUuid().toString(QUuid::WithoutBraces);

		QUrl url(QString::fromLatin1(kEntitlementsUrl));
		url.setQuery(QStringLiteral("requestId=%1").arg(m_requestId));

		const QString authHeader = QStringLiteral("Bearer %1").arg(m_credentials.minecraftAccessToken.accessToken);

		const auto headers = QList<Net::HeaderPair>{ { "Content-Type", "application/json" },
													 { "Accept", "application/json" },
													 { "Authorization", authHeader.toUtf8() } };

		m_response = std::make_shared<QByteArray>();
		m_request  = Net::Download::makeByteArray(url, m_response);
		m_request->addHeaderProxy(new Net::RawHeaderProxy(headers));

		m_task = NetJob::Ptr::create(QStringLiteral("GameEntitlements"), APPLICATION->network());
		m_task->setAskRetry(false);
		m_task->addNetAction(m_request);

		connect(m_task.get(), &Task::finished, this, &GameEntitlementsStep::onRequestCompleted);
		m_task->start();

		qDebug() << "Checking game entitlements...";
	}

	void GameEntitlementsStep::onRequestCompleted()
	{
		qCDebug(authCredentials()) << *m_response;

		// Entitlements fetch is non-critical - continue even on failure
		if (!parseEntitlementsResponse(*m_response))
		{
			qWarning() << "Failed to parse entitlements response; continuing without entitlements.";
		}

		emit completed(StepResult::Continue, tr("Got entitlements info."));
	}

	bool GameEntitlementsStep::parseEntitlementsResponse(const QByteArray& data)
	{
		QJsonParseError err;
		const auto doc = QJsonDocument::fromJson(data, &err);
		if (err.error != QJsonParseError::NoError || !doc.isObject())
		{
			qWarning() << "Failed to parse entitlements:" << err.errorString();
			return false;
		}

		const auto obj = doc.object();

		// Validate request ID matches
		const QString responseRequestId = obj.value(QStringLiteral("requestId")).toString();
		if (!responseRequestId.isEmpty() && responseRequestId != m_requestId)
		{
			qWarning() << "Entitlements request ID mismatch! Expected:" << m_requestId << "Got:" << responseRequestId;
		}

		// Parse items array for Minecraft entitlements
		const auto items  = obj.value(QStringLiteral("items")).toArray();
		bool hasMinecraft = false;
		bool hasGamePass  = false;

		for (const auto& itemVal : items)
		{
			const auto itemObj = itemVal.toObject();
			const QString name = itemObj.value(QStringLiteral("name")).toString();

			if (name == QStringLiteral("game_minecraft") || name == QStringLiteral("product_minecraft"))
			{
				hasMinecraft = true;
			}
			if (name == QStringLiteral("game_minecraft_bedrock"))
			{
				// Bedrock edition, not Java
			}
			if (name.contains(QStringLiteral("gamepass"), Qt::CaseInsensitive))
			{
				hasGamePass = true;
			}
		}

		m_credentials.entitlements.ownsMinecraft	= hasMinecraft || hasGamePass;
		m_credentials.entitlements.canPlayMinecraft = hasMinecraft || hasGamePass;
		m_credentials.entitlements.validity			= TokenValidity::Certain;

		return true;
	}

} // namespace projt::minecraft::auth
