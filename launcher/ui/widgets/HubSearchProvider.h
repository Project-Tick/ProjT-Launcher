// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 Project Tick
// SPDX-FileContributor: Project Tick Team
#pragma once

#include <QList>
#include <QString>
#include <QUrl>

struct HubSearchProvider
{
	QString id;
	QString displayName;
	QString templateUrl;
};

const QList<HubSearchProvider>& hubSearchProviders();
QString defaultHubSearchProviderId();
QString normalizedHubSearchProviderId(const QString& id);
QUrl hubSearchUrlForQuery(const QString& query, const QString& providerId);
QUrl resolveHubInput(const QString& input, const QString& providerId);
