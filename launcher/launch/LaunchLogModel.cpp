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

#include "LaunchLogModel.hpp"

namespace projt::launch
{
	LaunchLogModel::LaunchLogModel(QObject* parent) : QAbstractListModel(parent)
	{
		m_buffer.resize(m_limit);
		updateDefaultOverflowMessage();
	}

	int LaunchLogModel::rowCount(const QModelIndex& parent) const
	{
		if (parent.isValid())
		{
			return 0;
		}
		return m_size;
	}

	QVariant LaunchLogModel::data(const QModelIndex& index, int role) const
	{
		if (!index.isValid())
		{
			return {};
		}
		if (index.row() < 0 || index.row() >= m_size)
		{
			return {};
		}

		int realIndex = bufferIndex(index.row());
		if (role == Qt::DisplayRole || role == Qt::EditRole)
		{
			return m_buffer[realIndex].line;
		}
		if (role == LevelRole)
		{
			return m_buffer[realIndex].level;
		}
		return {};
	}

	void LaunchLogModel::append(MessageLevel::Enum level, QString line)
	{
		if (m_paused)
		{
			return;
		}
		if (m_size == m_limit)
		{
			if (m_hardLimit)
			{
				return;
			}
			beginRemoveRows(QModelIndex(), 0, 0);
			m_offset = (m_offset + 1) % m_limit;
			m_size--;
			endRemoveRows();
		}
		else if (m_hardLimit && m_size == m_limit - 1)
		{
			level = MessageLevel::Fatal;
			line  = m_overflowText;
		}

		int insertRow = m_size;
		int slot	  = (m_offset + m_size) % m_limit;

		beginInsertRows(QModelIndex(), insertRow, insertRow);
		m_buffer[slot].level = level;
		m_buffer[slot].line	 = line;
		m_size++;
		endInsertRows();
	}

	void LaunchLogModel::suspend(bool suspend)
	{
		m_paused = suspend;
	}

	bool LaunchLogModel::suspended() const
	{
		return m_paused;
	}

	void LaunchLogModel::clear()
	{
		beginResetModel();
		m_offset = 0;
		m_size	 = 0;
		endResetModel();
	}

	QString LaunchLogModel::toPlainText() const
	{
		QString out;
		out.reserve(m_size * 80);
		for (int i = 0; i < m_size; ++i)
		{
			out.append(m_buffer[bufferIndex(i)].line);
			out.append('\n');
		}
		out.squeeze();
		return out;
	}

	void LaunchLogModel::setMaxLines(int maxLines)
	{
		if (maxLines == m_limit || maxLines <= 0)
		{
			return;
		}

		beginResetModel();
		QVector<LogEntry> newRing;
		newRing.resize(maxLines);
		int keep = qMin(m_size, maxLines);
		for (int i = 0; i < keep; ++i)
		{
			int src	   = (m_offset + m_size - keep + i) % m_limit;
			newRing[i] = m_buffer[src];
		}
		m_buffer.swap(newRing);
		m_limit	 = maxLines;
		m_offset = 0;
		m_size	 = keep;
		endResetModel();
	}

	int LaunchLogModel::getMaxLines() const
	{
		return m_limit;
	}

	void LaunchLogModel::setStopOnOverflow(bool stop)
	{
		m_hardLimit = stop;
	}

	void LaunchLogModel::setOverflowMessage(const QString& overflowMessage)
	{
		m_overflowText = overflowMessage;
	}

	bool LaunchLogModel::isOverflow() const
	{
		return m_size >= m_limit && m_hardLimit;
	}

	void LaunchLogModel::setLineWrap(bool state)
	{
		m_wrapLines = state;
	}

	bool LaunchLogModel::wrapLines() const
	{
		return m_wrapLines;
	}

	void LaunchLogModel::setColorLines(bool state)
	{
		m_colorize = state;
	}

	bool LaunchLogModel::colorLines() const
	{
		return m_colorize;
	}

	MessageLevel::Enum LaunchLogModel::previousLevel() const
	{
		if (m_size == 0)
		{
			return MessageLevel::Unknown;
		}
		int lastIndex = bufferIndex(m_size - 1);
		return m_buffer[lastIndex].level;
	}

	void LaunchLogModel::updateDefaultOverflowMessage()
	{
		m_overflowText = tr("Stopped watching the game log because the log length surpassed %1 lines.\n"
							"You may have to fix your mods because the game is still logging to files and"
							" likely wasting harddrive space at an alarming rate!")
							 .arg(m_limit);
	}

	int LaunchLogModel::bufferIndex(int row) const
	{
		return (m_offset + row) % m_limit;
	}
} // namespace projt::launch
