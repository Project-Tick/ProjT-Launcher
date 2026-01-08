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
#include "EntitlementsStep.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QList>
#include <QNetworkRequest>
#include <QUrl>
#include <QUuid>
#include <memory>

#include "Application.h"
#include "Logging.h"
#include "minecraft/auth/Parsers.h"
#include "net/Download.h"
#include "net/NetJob.h"
#include "net/RawHeaderProxy.h"
#include "tasks/Task.h"

EntitlementsStep::EntitlementsStep(AccountData* data) : AuthStep(data) {}

QString EntitlementsStep::describe()
{
    return tr("Determining game ownership.");
}

void EntitlementsStep::perform()
{
    auto uuid = QUuid::createUuid();
    m_entitlements_request_id = uuid.toString().remove('{').remove('}');

    QUrl url("https://api.minecraftservices.com/entitlements/license?requestId=" + m_entitlements_request_id);
    auto headers = QList<Net::HeaderPair>{ { "Content-Type", "application/json" },
                                           { "Accept", "application/json" },
                                           { "Authorization", QString("Bearer %1").arg(m_data->yggdrasilToken.token).toUtf8() } };

    m_response.reset(new QByteArray());
    m_request = Net::Download::makeByteArray(url, m_response);
    m_request->addHeaderProxy(new Net::RawHeaderProxy(headers));

    m_task.reset(new NetJob("EntitlementsStep", APPLICATION->network()));
    m_task->setAskRetry(false);
    m_task->addNetAction(m_request);

    connect(m_task.get(), &Task::finished, this, &EntitlementsStep::onRequestDone);

    m_task->start();
    qDebug() << "Getting entitlements...";
}

void EntitlementsStep::onRequestDone()
{
    qCDebug(authCredentials()) << *m_response;

    // Parse JSON to validate request ID matches
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(*m_response, &jsonError);
    if (!jsonError.error) {
        auto obj = doc.object();
        QString responseRequestId = obj.value("requestId").toString();
        if (!responseRequestId.isEmpty() && responseRequestId != m_entitlements_request_id) {
            qWarning() << "Entitlements response requestId mismatch! Expected:" << m_entitlements_request_id << "Got:" << responseRequestId;
        }
    }

    Parsers::parseMinecraftEntitlements(*m_response, m_data->minecraftEntitlement);

    emit finished(AccountTaskState::STATE_WORKING, tr("Got entitlements"));
}
