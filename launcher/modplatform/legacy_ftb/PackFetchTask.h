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

#include <QByteArray>
#include <QObject>
#include <QTemporaryDir>
#include <memory>
#include "PackHelpers.h"
#include "net/NetJob.h"

namespace LegacyFTB
{

	class PackFetchTask : public QObject
	{
		Q_OBJECT

	  public:
		PackFetchTask(shared_qobject_ptr<QNetworkAccessManager> network) : QObject(nullptr), m_network(network) {};
		virtual ~PackFetchTask() = default;

		void fetch();
		void fetchPrivate(const QStringList& toFetch);

	  private:
		shared_qobject_ptr<QNetworkAccessManager> m_network;
		NetJob::Ptr jobPtr;

		std::shared_ptr<QByteArray> publicModpacksXmlFileData	  = std::make_shared<QByteArray>();
		std::shared_ptr<QByteArray> thirdPartyModpacksXmlFileData = std::make_shared<QByteArray>();

		bool parseAndAddPacks(QByteArray& data, PackType packType, ModpackList& list);
		ModpackList publicPacks;
		ModpackList thirdPartyPacks;

	  protected slots:
		void fileDownloadFinished();
		void fileDownloadFailed(QString reason);
		void fileDownloadAborted();

	  signals:
		void finished(ModpackList publicPacks, ModpackList thirdPartyPacks);
		void failed(QString reason);
		void aborted();

		void privateFileDownloadFinished(const Modpack& modpack);
		void privateFileDownloadFailed(QString reason, QString packCode);
	};

} // namespace LegacyFTB
