// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 Project Tick
// SPDX-FileContributor: Project Tick Team

#include "HubSearchProvider.h"

#include <QRegularExpression>

namespace
{
	const QList<HubSearchProvider> kProviders{
		{ QStringLiteral("duckduckgo"), QStringLiteral("DuckDuckGo"), QStringLiteral("https://duckduckgo.com/?q=%1") },
		{ QStringLiteral("google"), QStringLiteral("Google"), QStringLiteral("https://www.google.com/search?q=%1") },
		{ QStringLiteral("brave"), QStringLiteral("Brave Search"), QStringLiteral("https://search.brave.com/search?q=%1") },
		{ QStringLiteral("bing"), QStringLiteral("Bing"), QStringLiteral("https://www.bing.com/search?q=%1") },
		{ QStringLiteral("startpage"), QStringLiteral("Startpage"), QStringLiteral("https://www.startpage.com/do/search?query=%1") },
	};

	bool looksLikeExplicitUrl(const QString& input)
	{
		static const QRegularExpression kSchemePattern(QStringLiteral(R"(^[a-zA-Z][a-zA-Z0-9+\-.]*:)"));

		if (input.startsWith(QLatin1Char('/')) || input.startsWith(QStringLiteral("./"))
			|| input.startsWith(QStringLiteral("../")) || input.startsWith(QLatin1Char('~')))
		{
			return true;
		}

		if (kSchemePattern.match(input).hasMatch())
		{
			return true;
		}

		if (input.compare(QStringLiteral("localhost"), Qt::CaseInsensitive) == 0)
		{
			return true;
		}

		return input.contains(QLatin1Char('.')) || input.contains(QLatin1Char(':'));
	}
}

const QList<HubSearchProvider>& hubSearchProviders()
{
	return kProviders;
}

QString defaultHubSearchProviderId()
{
	return QStringLiteral("duckduckgo");
}

QString normalizedHubSearchProviderId(const QString& id)
{
	for (const auto& provider : kProviders)
	{
		if (provider.id == id)
		{
			return provider.id;
		}
	}
	return defaultHubSearchProviderId();
}

QUrl hubSearchUrlForQuery(const QString& query, const QString& providerId)
{
	const QString normalizedId = normalizedHubSearchProviderId(providerId);
	for (const auto& provider : kProviders)
	{
		if (provider.id == normalizedId)
		{
			return QUrl(provider.templateUrl.arg(QString::fromUtf8(QUrl::toPercentEncoding(query))));
		}
	}

	return QUrl(kProviders.constFirst().templateUrl.arg(QString::fromUtf8(QUrl::toPercentEncoding(query))));
}

QUrl resolveHubInput(const QString& input, const QString& providerId)
{
	const QString trimmed = input.trimmed();
	if (trimmed.isEmpty())
	{
		return {};
	}

	if (looksLikeExplicitUrl(trimmed))
	{
		const QUrl url = QUrl::fromUserInput(trimmed);
		if (url.isValid())
		{
			return url;
		}
	}

	return hubSearchUrlForQuery(trimmed, providerId);
}
