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

#include <QCryptographicHash>
#include <QFuture>
#include <QFutureWatcher>
#include <QString>

#include "modplatform/ModIndex.h"
#include "tasks/Task.h"

namespace Hashing
{

	enum class Algorithm
	{
		Md4,
		Md5,
		Sha1,
		Sha256,
		Sha512,
		Murmur2,
		Unknown
	};

	QString algorithmToString(Algorithm type);
	Algorithm algorithmFromString(QString type);
	QString hash(QIODevice* device, Algorithm type);
	QString hash(QString fileName, Algorithm type);
	QString hash(QByteArray data, Algorithm type);

	class Hasher : public Task
	{
		Q_OBJECT
	  public:
		using Ptr = shared_qobject_ptr<Hasher>;

		Hasher(QString file_path, Algorithm alg) : m_path(file_path), m_alg(alg)
		{}
		Hasher(QString file_path, QString alg) : Hasher(file_path, algorithmFromString(alg))
		{}

		bool abort() override;

		void executeTask() override;

		QString getResult() const
		{
			return m_result;
		};
		QString getPath() const
		{
			return m_path;
		};

	  signals:
		void resultsReady(QString hash);

	  private:
		QString m_result;
		QString m_path;
		Algorithm m_alg;

		QFuture<QString> m_future;
		QFutureWatcher<QString> m_watcher;
	};

	Hasher::Ptr createHasher(QString file_path, ModPlatform::ResourceProvider provider);
	Hasher::Ptr createHasher(QString file_path, QString type);

} // namespace Hashing
