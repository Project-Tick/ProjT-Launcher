// SPDX-License-Identifier: GPL-3.0-only AND Apache-2.0
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
 *
 * === Upstream License Block (Do Not Modify) ==============================
 *
 * Copyright 2020-2021 Jamie Mansfield <jmansfield@cadixdev.org>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ======================================================================== */

#pragma once

#include <QAbstractListModel>

#include <modplatform/atlauncher/ATLPackIndex.h>
#include <QIcon>
#include "net/NetJob.h"

namespace Atl
{

	using LogoMap	   = QMap<QString, QIcon>;
	using LogoCallback = std::function<void(QString)>;

	class ListModel : public QAbstractListModel
	{
		Q_OBJECT

	  public:
		ListModel(QObject* parent);
		virtual ~ListModel();

		int rowCount(const QModelIndex& parent) const override;
		int columnCount(const QModelIndex& parent) const override;
		QVariant data(const QModelIndex& index, int role) const override;

		void request();

		void getLogo(const QString& logo, const QString& logoUrl, LogoCallback callback);

	  private slots:
		void requestFinished();
		void requestFailed(QString reason);

		void logoFailed(QString logo);
		void logoLoaded(QString logo, QIcon out);

	  private:
		void requestLogo(QString file, QString url);

	  private:
		QList<ATLauncher::IndexedPack> modpacks;

		QStringList m_failedLogos;
		QStringList m_loadingLogos;
		LogoMap m_logoMap;
		QMap<QString, LogoCallback> waitingCallbacks;

		NetJob::Ptr jobPtr;
		std::shared_ptr<QByteArray> response = std::make_shared<QByteArray>();
	};

} // namespace Atl
