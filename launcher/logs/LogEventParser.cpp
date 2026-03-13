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

#include "LogEventParser.hpp"

#include <QRegularExpression>

using namespace Qt::Literals::StringLiterals;

namespace projt::logs
{
	void LogEventParser::pushLine(QAnyStringView data)
	{
		// If we have holdover data from incomplete XML, prepend it
		if (!m_holdover.isEmpty())
		{
			m_buffer = m_holdover;
			m_buffer.append('\n');
			m_holdover.clear();
		}
		// Otherwise, clear buffer before adding new data to prevent duplicates
		else if (!m_buffer.isEmpty())
		{
			m_buffer.clear();
		}
		m_buffer.append(data.toString());
	}

	std::optional<LogEventParser::ParseFailure> LogEventParser::lastError() const
	{
		return m_error;
	}

	void LogEventParser::captureError()
	{
		m_error = {
			m_reader.errorString(),
			m_reader.error(),
		};
	}

	void LogEventParser::clearError()
	{
		m_error = {};
	}

	bool LogEventParser::isPotentialLog4j(QStringView view) const
	{
		static const QString marker = QStringLiteral("<log4j:event");
		if (view.isEmpty() || view.front() != '<')
		{
			return false;
		}
		const QString lowered = view.toString().toLower();
		return marker.startsWith(lowered) || lowered.startsWith(marker);
	}

	bool LogEventParser::looksCompleteLog4j() const
	{
		if (m_buffer.isEmpty())
		{
			return false;
		}
		QXmlStreamReader probe;
		probe.setNamespaceProcessing(false);
		probe.addData(m_buffer);
		if (!probe.readNextStartElement())
		{
			return false;
		}
		if (probe.qualifiedName().compare("log4j:Event"_L1, Qt::CaseInsensitive) != 0)
		{
			return false;
		}

		int depth = 1;
		while (!probe.atEnd())
		{
			auto token = probe.readNext();
			switch (token)
			{
				case QXmlStreamReader::TokenType::StartElement: depth += 1; break;
				case QXmlStreamReader::TokenType::EndElement: depth -= 1; break;
				case QXmlStreamReader::TokenType::EndDocument: return false;
				default: break;
			}
			if (probe.hasError())
			{
				return false;
			}
			if (depth == 0)
			{
				return true;
			}
		}
		return false;
	}

	std::optional<LogEventParser::LogRecord> LogEventParser::readEventAttributes()
	{
		LogRecord entry;
		entry.level		= MessageLevel::Info;
		auto attributes = m_reader.attributes();

		for (const auto& attr : attributes)
		{
			const auto name	 = attr.name();
			const auto value = attr.value();
			if (name == "logger"_L1)
			{
				entry.logger = value.trimmed().toString();
			}
			else if (name == "timestamp"_L1)
			{
				if (value.trimmed().isEmpty())
				{
					m_reader.raiseError("log4j:Event missing timestamp attribute");
					return {};
				}
				entry.timestamp = QDateTime::fromSecsSinceEpoch(value.trimmed().toLongLong());
			}
			else if (name == "level"_L1)
			{
				entry.levelText = value.trimmed().toString();
				entry.level		= MessageLevel::getLevel(entry.levelText);
			}
			else if (name == "thread"_L1)
			{
				entry.thread = value.trimmed().toString();
			}
		}

		if (entry.logger.isEmpty())
		{
			m_reader.raiseError("log4j:Event missing logger attribute");
			return {};
		}

		return entry;
	}

	std::optional<LogEventParser::Item> LogEventParser::parseLog4jEvent()
	{
		m_reader.clear();
		m_reader.setNamespaceProcessing(false);
		m_reader.addData(m_buffer);

		m_reader.readNextStartElement();
		if (m_reader.qualifiedName().compare("log4j:Event"_L1, Qt::CaseInsensitive) != 0)
		{
			return {};
		}

		auto entry = readEventAttributes();
		if (!entry.has_value())
		{
			captureError();
			return {};
		}

		bool haveMessage = false;
		while (!m_reader.atEnd())
		{
			auto token = m_reader.readNext();
			if (token == QXmlStreamReader::TokenType::StartElement)
			{
				if (m_reader.qualifiedName().compare("log4j:Message"_L1, Qt::CaseInsensitive) == 0)
				{
					entry->message = m_reader.readElementText(QXmlStreamReader::IncludeChildElements);
					if (entry->message.endsWith(QLatin1Char('\n')))
					{
						entry->message.chop(1);
					}
					haveMessage = true;
				}
			}
			else if (token == QXmlStreamReader::TokenType::EndElement)
			{
				if (m_reader.qualifiedName().compare("log4j:Event"_L1, Qt::CaseInsensitive) == 0)
				{
					if (!haveMessage)
					{
						m_reader.raiseError("log4j:Event missing message element");
						captureError();
						return {};
					}
					auto consumed = m_reader.characterOffset();
					if (consumed > 0 && consumed <= m_buffer.length())
					{
						m_buffer = m_buffer.right(m_buffer.length() - consumed);
					}
					clearError();
					return entry.value();
				}
			}
			else if (token == QXmlStreamReader::TokenType::EndDocument)
			{
				return {};
			}

			if (m_reader.hasError())
			{
				captureError();
				return {};
			}
		}

		return {};
	}

	std::optional<LogEventParser::Item> LogEventParser::popNext()
	{
		clearError();

		if (m_buffer.isEmpty())
		{
			return {};
		}

		// Ignore leading whitespace before a log4j XML event.
		{
			qsizetype firstNonSpace = -1;
			for (qsizetype i = 0; i < m_buffer.size(); ++i)
			{
				if (!m_buffer.at(i).isSpace())
				{
					firstNonSpace = i;
					break;
				}
			}
			if (firstNonSpace > 0)
			{
				const QStringView slice = QStringView{ m_buffer }.sliced(firstNonSpace);
				if (!slice.isEmpty() && slice.front() == QLatin1Char('<') && isPotentialLog4j(slice))
				{
					m_buffer = m_buffer.mid(firstNonSpace);
				}
			}
		}

		if (m_buffer.trimmed().isEmpty())
		{
			auto text = QString(m_buffer);
			m_buffer.clear();
			return RawLine{ text };
		}

		if (isPotentialLog4j(QStringView(m_buffer)))
		{
			if (!looksCompleteLog4j())
			{
				m_holdover = m_buffer;
				return PendingChunk{ m_buffer };
			}
			return parseLog4jEvent();
		}

		const QStringView view(m_buffer);
		qsizetype offset = 0;
		while (offset < view.length())
		{
			auto rel = view.sliced(offset).indexOf(QLatin1Char('<'));
			if (rel < 0)
			{
				break;
			}
			auto pos   = offset + rel;
			auto slice = view.sliced(pos);
			if (isPotentialLog4j(slice))
			{
				if (pos > 0)
				{
					auto text = m_buffer.left(pos);
					m_buffer  = m_buffer.right(m_buffer.length() - pos);
					return RawLine{ text };
				}
				m_holdover = m_buffer;
				return PendingChunk{ m_buffer };
			}
			offset = pos + 1;
		}

		auto text = QString(m_buffer);
		m_buffer.clear();
		return RawLine{ text };
	}

	QList<LogEventParser::Item> LogEventParser::drainAvailable()
	{
		QList<LogEventParser::Item> items;
		bool keepGoing = true;
		while (keepGoing)
		{
			auto item = popNext();
			if (m_error.has_value())
			{
				return {};
			}
			if (item.has_value())
			{
				if (std::holds_alternative<PendingChunk>(item.value()))
				{
					break;
				}
				items.push_back(item.value());
			}
			else
			{
				keepGoing = false;
			}
		}
		return items;
	}

	MessageLevel::Enum LogEventParser::guessLevelFromLine(const QString& line, MessageLevel::Enum fallback)
	{
		static const QRegularExpression kLogHeader("^\\[(?<timestamp>[0-9:]+)\\] \\[[^/]+/(?<level>[^\\]]+)\\]");
		auto match = kLogHeader.match(line);
		if (match.hasMatch())
		{
			auto levelText = match.captured("level");
			fallback	   = MessageLevel::getLevel(levelText);
		}
		else
		{
			if (line.contains("[INFO]") || line.contains("[CONFIG]") || line.contains("[FINE]")
				|| line.contains("[FINER]") || line.contains("[FINEST]"))
			{
				fallback = MessageLevel::Info;
			}
			if (line.contains("[SEVERE]") || line.contains("[STDERR]"))
			{
				fallback = MessageLevel::Error;
			}
			if (line.contains("[WARNING]"))
			{
				fallback = MessageLevel::Warning;
			}
			if (line.contains("[DEBUG]"))
			{
				fallback = MessageLevel::Debug;
			}
		}
		if (fallback != MessageLevel::Unknown)
		{
			return fallback;
		}
		if (line.contains("overwriting existing"))
		{
			return MessageLevel::Fatal;
		}
		return MessageLevel::Info;
	}
} // namespace projt::logs
