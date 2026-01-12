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
#pragma once

#include <QAbstractListModel>
#include <QString>
#include "MessageLevel.h"

class LogModel : public QAbstractListModel
{
	Q_OBJECT
  public:
	explicit LogModel(QObject* parent = 0);

	int rowCount(const QModelIndex& parent = QModelIndex()) const;
	QVariant data(const QModelIndex& index, int role) const;

	void append(MessageLevel::Enum, QString line);
	void clear();

	void suspend(bool suspend);
	bool suspended();

	QString toPlainText();

	int getMaxLines();
	void setMaxLines(int maxLines);
	void setStopOnOverflow(bool stop);
	void setOverflowMessage(const QString& overflowMessage);
	bool isOverFlow();

	void setLineWrap(bool state);
	bool wrapLines() const;
	void setColorLines(bool state);
	bool colorLines() const;

	MessageLevel::Enum previousLevel();

	enum Roles
	{
		LevelRole = Qt::UserRole
	};

  private /* types */:
	struct entry
	{
		MessageLevel::Enum level = MessageLevel::Enum::Unknown;
		QString line;
	};

  private: /* data */
	QList<entry> m_content;
	int m_maxLines = 1000;
	// first line in the circular buffer
	int m_firstLine = 0;
	// number of lines occupied in the circular buffer
	int m_numLines			  = 0;
	bool m_stopOnOverflow	  = false;
	QString m_overflowMessage = "OVERFLOW";
	bool m_suspended		  = false;
	bool m_lineWrap			  = true;
	bool m_colorLines		  = true;

  private:
	void updateOverflowMessage();
	Q_DISABLE_COPY(LogModel)
};
