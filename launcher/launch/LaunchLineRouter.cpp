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

#include "LaunchLineRouter.hpp"

#include <QObject>
#include <variant>

#include "LaunchLogModel.hpp"

namespace projt::launch
{
	QString LaunchLineRouter::applyRedactions(QString input, const QMap<QString, QString>& redactions) const
	{
		for (auto it = redactions.begin(); it != redactions.end(); ++it)
		{
			input.replace(it.key(), it.value());
		}
		return input;
	}

	bool LaunchLineRouter::parseStructured(const QString& line,
										   MessageLevel::Enum level,
										   LaunchLogModel& model,
										   const QMap<QString, QString>& redactions)
	{
		projt::logs::LogEventParser* parser = nullptr;
		switch (level)
		{
			case MessageLevel::StdErr: parser = &m_stderrParser; break;
			case MessageLevel::StdOut: parser = &m_stdoutParser; break;
			default: return false;
		}

		parser->pushLine(line);
		auto items = parser->drainAvailable();
		if (auto err = parser->lastError(); err.has_value())
		{
			model.append(MessageLevel::Error, QObject::tr("Log parser error: %1").arg(err.value().message));
			return false;
		}

		if (items.isEmpty())
		{
			return true;
		}

		for (const auto& item : items)
		{
			if (std::holds_alternative<projt::logs::LogEventParser::LogRecord>(item))
			{
				auto entry = std::get<projt::logs::LogEventParser::LogRecord>(item);
				auto msg   = QString("[%1] [%2/%3] [%4]: %5")
							   .arg(entry.timestamp.toString("HH:mm:ss"))
							   .arg(entry.thread)
							   .arg(entry.levelText)
							   .arg(entry.logger)
							   .arg(entry.message);
				msg = applyRedactions(msg, redactions);
				model.append(entry.level, msg);
				continue;
			}
			if (std::holds_alternative<projt::logs::LogEventParser::RawLine>(item))
			{
				auto msg	  = std::get<projt::logs::LogEventParser::RawLine>(item).text;
				auto resolved = projt::logs::LogEventParser::guessLevelFromLine(msg, model.previousLevel());
				msg			  = applyRedactions(msg, redactions);
				model.append(resolved, msg);
			}
		}

		return true;
	}

	void LaunchLineRouter::routeLine(QString line,
									 MessageLevel::Enum defaultLevel,
									 LaunchLogModel& model,
									 const QMap<QString, QString>& redactions)
	{
		if (parseStructured(line, defaultLevel, model, redactions))
		{
			return;
		}

		auto embedded = MessageLevel::fromLine(line);
		if (embedded != MessageLevel::Unknown)
		{
			defaultLevel = embedded;
		}

		if (defaultLevel == MessageLevel::Unknown)
		{
			defaultLevel = projt::logs::LogEventParser::guessLevelFromLine(line, model.previousLevel());
		}

		line = applyRedactions(line, redactions);
		model.append(defaultLevel, line);
	}
} // namespace projt::launch
