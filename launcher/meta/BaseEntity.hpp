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

#include <QJsonObject>
#include <QObject>
#include <QUrl>
#include <memory>

#include "net/Mode.h"
#include "net/NetJob.h"
#include "tasks/Task.h"

namespace projt::meta
{

	class EntityLoader;

	/**
	 * @brief Cacheable metadata entity loaded from local cache or remote.
	 */
	class MetaEntity
	{
		friend class EntityLoader;

	  public:
		using Ptr = std::shared_ptr<MetaEntity>;

		enum class State
		{
			Pending,
			Cached,
			Synchronized
		};

		virtual ~MetaEntity() = default;

		virtual QString cacheFilePath() const = 0;
		virtual QUrl remoteUrl() const;

		State state() const
		{
			return m_state;
		}
		bool isReady() const
		{
			return m_state != State::Pending;
		}

		void setExpectedChecksum(const QString& sha256)
		{
			m_expectedSha256 = sha256;
		}
		QString expectedChecksum() const
		{
			return m_expectedSha256;
		}

		virtual void loadFromJson(const QJsonObject& json) = 0;

		[[nodiscard]] Task::Ptr createLoadTask(Net::Mode mode = Net::Mode::Online);

	  protected:
		State m_state = State::Pending;
		QString m_expectedSha256;
		QString m_actualSha256;
		Task::Ptr m_activeTask;
	};

	/**
	 * @brief Task for loading a MetaEntity.
	 */
	class EntityLoader : public Task
	{
		Q_OBJECT

	  public:
		explicit EntityLoader(MetaEntity* target, Net::Mode mode);
		~EntityLoader() override = default;

	  protected:
		void executeTask() override;
		bool canAbort() const override;
		bool abort() override;

	  private:
		void attemptLocalLoad();
		void initiateRemoteFetch();
		bool loadCachedMetadata(const QString& cachePath, QString* errorOut = nullptr);
		void handleRemoteFailure(const QString& reason);
		void finalizeLoad(MetaEntity::State newState);

		MetaEntity* m_target;
		Net::Mode m_mode;
		NetJob::Ptr m_netTask;
		bool m_canUseLocalFallback = false;
	};

} // namespace projt::meta
