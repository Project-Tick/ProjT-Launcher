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

#include "LogRedactor.hpp"

#include <QRegularExpression>
#include <QVector>

namespace projt::logs
{
	struct RedactionRule
	{
		QRegularExpression pattern;
		QString replacement;
	};

	static QVector<RedactionRule> buildDefaultRules()
	{
		QVector<RedactionRule> rules;
		rules.reserve(7);
		rules.push_back(
			{ QRegularExpression("C:\\\\Users\\\\([^\\\\]+)\\\\", QRegularExpression::CaseInsensitiveOption),
			  "C:\\Users\\********\\" });
		rules.push_back({ QRegularExpression("C:\\/Users\\/([^\\/]+)\\/", QRegularExpression::CaseInsensitiveOption),
						  "C:/Users/********/" });
		rules.push_back(
			{ QRegularExpression("(?<!\\\\w)\\/home\\/[^\\/]+\\/", QRegularExpression::CaseInsensitiveOption),
			  "/home/********/" });
		rules.push_back(
			{ QRegularExpression("(?<!\\\\w)\\/Users\\/[^\\/]+\\/", QRegularExpression::CaseInsensitiveOption),
			  "/Users/********/" });
		rules.push_back({ QRegularExpression("\\(Session ID is [^\\)]+\\)", QRegularExpression::CaseInsensitiveOption),
						  "(Session ID is <SESSION_TOKEN>)" });
		rules.push_back(
			{ QRegularExpression("new refresh token: \"[^\"]+\"", QRegularExpression::CaseInsensitiveOption),
			  "new refresh token: \"<TOKEN>\"" });
		rules.push_back(
			{ QRegularExpression("\"device_code\" :  \"[^\"]+\"", QRegularExpression::CaseInsensitiveOption),
			  "\"device_code\" :  \"<DEVICE_CODE>\"" });

		for (auto& rule : rules)
		{
			rule.pattern.optimize();
		}
		return rules;
	}

	static const QVector<RedactionRule>& ruleset()
	{
		static const QVector<RedactionRule> rules = buildDefaultRules();
		return rules;
	}

	void redactLog(QString& text)
	{
		for (const auto& rule : ruleset())
		{
			text.replace(rule.pattern, rule.replacement);
		}
	}

	QString redactCopy(const QString& text)
	{
		QString output = text;
		redactLog(output);
		return output;
	}
} // namespace projt::logs
