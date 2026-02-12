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

#include <QCoreApplication>
#include <QDataStream>
#include <QList>
#include <QLocalSocket>
#include <QString>

#include "FileSystem.h"

namespace projt::filelink
{
	class FileLinkApp final : public QCoreApplication
	{
		Q_OBJECT
	  public:
		enum class Status
		{
			Starting,
			Failed,
			Succeeded,
			Initialized
		};

		explicit FileLinkApp(int& argc, char** argv);
		~FileLinkApp() override;

		Status status() const noexcept
		{
			return m_status;
		}

	  private:
		void joinServer(const QString& serverName);
		void readPathPairs();
		void runLink();
		void sendResults();

		Status m_status		= Status::Starting;
		bool m_useHardLinks = false;

		QLocalSocket m_socket;
		QDataStream m_input;
		quint32 m_blockSize = 0;

		QList<FS::LinkPair> m_linksToMake;
		QList<FS::LinkResult> m_linkResults;

#if defined Q_OS_WIN32
		bool m_consoleAttached = false;
#endif
	};
} // namespace projt::filelink
