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

#pragma once

#include <QAnyStringView>
#include <QDateTime>
#include <QList>
#include <QString>
#include <QStringView>
#include <QXmlStreamReader>
#include <optional>
#include <variant>

#include "MessageLevel.h"

namespace projt::logs
{
	class LogEventParser
	{
	  public:
		struct LogRecord
		{
			QString logger;
			MessageLevel::Enum level = MessageLevel::Unknown;
			QString levelText;
			QDateTime timestamp;
			QString thread;
			QString message;
		};

		struct RawLine
		{
			QString text;
		};

		struct PendingChunk
		{
			QString data;
		};

		struct ParseFailure
		{
			QString message;
			QXmlStreamReader::Error code = QXmlStreamReader::Error::NoError;
		};

		using Item = std::variant<LogRecord, RawLine, PendingChunk>;

		LogEventParser() = default;

		void pushLine(QAnyStringView data);
		std::optional<Item> popNext();
		QList<Item> drainAvailable();
		std::optional<ParseFailure> lastError() const;

		static MessageLevel::Enum guessLevelFromLine(const QString& line, MessageLevel::Enum fallback);

	  private:
		std::optional<LogRecord> readEventAttributes();
		std::optional<Item> parseLog4jEvent();
		bool isPotentialLog4j(QStringView view) const;
		bool looksCompleteLog4j() const;
		void captureError();
		void clearError();

		QString m_buffer;
		QString m_holdover;
		QXmlStreamReader m_reader;
		std::optional<ParseFailure> m_error;
	};
} // namespace projt::logs
