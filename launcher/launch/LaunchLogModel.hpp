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

#include <QAbstractListModel>
#include <QString>
#include <QVector>

#include "MessageLevel.h"

namespace projt::launch
{
	class LaunchLogModel : public QAbstractListModel
	{
		Q_OBJECT
	  public:
		explicit LaunchLogModel(QObject* parent = nullptr);

		int rowCount(const QModelIndex& parent = QModelIndex()) const override;
		QVariant data(const QModelIndex& index, int role) const override;

		void append(MessageLevel::Enum level, QString line);
		void clear();

		void suspend(bool suspend);
		bool suspended() const;

		QString toPlainText() const;

		int getMaxLines() const;
		void setMaxLines(int maxLines);
		void setStopOnOverflow(bool stop);
		void setOverflowMessage(const QString& overflowMessage);
		bool isOverflow() const;

		void setLineWrap(bool state);
		bool wrapLines() const;
		void setColorLines(bool state);
		bool colorLines() const;

		MessageLevel::Enum previousLevel() const;

		enum Roles
		{
			LevelRole = Qt::UserRole
		};

	  private:
		struct LogEntry
		{
			MessageLevel::Enum level = MessageLevel::Enum::Unknown;
			QString line;
		};

		void updateDefaultOverflowMessage();
		int bufferIndex(int row) const;

		QVector<LogEntry> m_buffer;
		int m_limit			   = 1000;
		int m_offset		   = 0;
		int m_size			   = 0;
		bool m_hardLimit	   = false;
		QString m_overflowText = "OVERFLOW";
		bool m_paused		   = false;
		bool m_wrapLines	   = true;
		bool m_colorize		   = true;
	};
} // namespace projt::launch
