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
#include <quazip/quazip.h>
#include <quazip/quazipdir.h>
#include "InstanceTask.h"
#include "PackHelpers.h"
#include "meta/Index.hpp"
#include "meta/Version.hpp"
#include "meta/VersionList.hpp"
#include "net/NetJob.h"

#include "net/NetJob.h"

#include <optional>

namespace LegacyFTB
{

	class PackInstallTask : public InstanceTask
	{
		Q_OBJECT

	  public:
		explicit PackInstallTask(shared_qobject_ptr<QNetworkAccessManager> network,
								 const Modpack& pack,
								 QString version);
		virtual ~PackInstallTask()
		{}

		bool canAbort() const override
		{
			return true;
		}
		bool abort() override;

	  protected:
		//! Entry point for tasks.
		virtual void executeTask() override;

	  private:
		void downloadPack();
		void unzip();
		void install();

	  private slots:

		void onUnzipFinished();
		void onUnzipCanceled();

	  private: /* data */
		shared_qobject_ptr<QNetworkAccessManager> m_network;
		bool abortable = false;
		std::unique_ptr<QuaZip> m_packZip;
		QFuture<std::optional<QStringList>> m_extractFuture;
		QFutureWatcher<std::optional<QStringList>> m_extractFutureWatcher;
		NetJob::Ptr netJobContainer;
		QString archivePath;

		Modpack m_pack;
		QString m_version;
	};

} // namespace LegacyFTB
