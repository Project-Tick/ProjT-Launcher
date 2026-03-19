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

#include "BaseEntity.hpp"

#include <QDir>
#include <QFile>

#include "Application.h"
#include "BuildConfig.h"
#include "Exception.h"
#include "FileSystem.h"
#include "Json.h"
#include "modplatform/helpers/HashUtils.h"
#include "net/ApiDownload.h"
#include "net/ChecksumValidator.h"
#include "net/HttpMetaCache.h"

namespace projt::meta
{

	namespace
	{

		/**
		 * @brief Validator that parses downloaded JSON into the target entity.
		 */
		class JsonParseValidator : public Net::Validator
		{
		  public:
			explicit JsonParseValidator(MetaEntity* entity) : m_entity(entity)
			{}

			bool init(QNetworkRequest&) override
			{
				m_buffer.clear();
				return true;
			}

			bool write(QByteArray& chunk) override
			{
				m_buffer.append(chunk);
				return true;
			}

			bool abort() override
			{
				m_buffer.clear();
				return true;
			}

			bool validate(QNetworkReply&) override
			{
				QString filename = m_entity->cacheFilePath();
				try
				{
					QJsonDocument doc = Json::requireDocument(m_buffer, filename);
					QJsonObject root  = Json::requireObject(doc, filename);
					m_entity->loadFromJson(root);
					return true;
				}
				catch (const Exception& ex)
				{
					qWarning() << "Failed to parse metadata:" << ex.cause();
					return false;
				}
			}

		  private:
			QByteArray m_buffer;
			MetaEntity* m_entity;
		};

	} // anonymous namespace

	QUrl MetaEntity::remoteUrl() const
	{
		auto settings	 = APPLICATION->settings();
		QString override = settings->get("MetaURLOverride").toString();

		QString baseUrl = override.isEmpty() ? BuildConfig.META_URL : override;
		return QUrl(baseUrl).resolved(cacheFilePath());
	}

	Task::Ptr MetaEntity::createLoadTask(Net::Mode mode)
	{
		if (m_activeTask && m_activeTask->isRunning())
			return m_activeTask;

		m_activeTask = Task::Ptr(new EntityLoader(this, mode));
		return m_activeTask;
	}

	// EntityLoader implementation

	EntityLoader::EntityLoader(MetaEntity* target, Net::Mode mode) : m_target(target), m_mode(mode)
	{}

	void EntityLoader::executeTask()
	{
		attemptLocalLoad();
	}

	void EntityLoader::attemptLocalLoad()
	{
		QString cachePath = QDir("meta").absoluteFilePath(m_target->cacheFilePath());

		if (!QFile::exists(cachePath))
		{
			// No local cache, need remote fetch
			if (m_mode == Net::Mode::Offline)
			{
				emitFailed(tr("Metadata not available offline: %1").arg(m_target->cacheFilePath()));
				return;
			}
			initiateRemoteFetch();
			return;
		}

		QString cacheError;

		if (m_mode == Net::Mode::Online)
		{
			// In online mode, always revalidate metadata against the remote cache entry on first load.
			// Relying only on the cached file plus the last known expected checksum can leave us stuck
			// on stale package manifests across launcher restarts.
			if (loadCachedMetadata(cachePath, &cacheError))
			{
				m_target->m_state	   = MetaEntity::State::Cached;
				m_canUseLocalFallback = true;
			}
			else
			{
				qDebug() << "Cache parse failed for" << cachePath << ":" << cacheError;
				FS::deletePath(cachePath);
				m_target->m_state	   = MetaEntity::State::Pending;
				m_canUseLocalFallback = false;
			}
			initiateRemoteFetch();
			return;
		}

		if (loadCachedMetadata(cachePath, &cacheError))
		{
			m_target->m_state = MetaEntity::State::Cached;
			emitSucceeded();
			return;
		}

		qDebug() << "Cache parse failed for" << cachePath << ":" << cacheError;
		FS::deletePath(cachePath);
		m_target->m_state = MetaEntity::State::Pending;

		if (m_mode == Net::Mode::Offline)
		{
			emitFailed(tr("Cached metadata corrupted and offline mode active"));
			return;
		}

		initiateRemoteFetch();
	}

	void EntityLoader::initiateRemoteFetch()
	{
		setStatus(tr("Downloading metadata: %1").arg(m_target->cacheFilePath()));

		m_netTask = NetJob::Ptr(new NetJob(tr("Fetch %1").arg(m_target->cacheFilePath()), APPLICATION->network()));

		auto cacheEntry = APPLICATION->metacache()->resolveEntry("meta", m_target->cacheFilePath());
		cacheEntry->setStale(true);

		auto download = Net::ApiDownload::makeCached(m_target->remoteUrl(), cacheEntry);

		// Add checksum validator if available
		if (!m_target->m_expectedSha256.isEmpty())
		{
			download->addValidator(new Net::ChecksumValidator(QCryptographicHash::Sha256, m_target->m_expectedSha256));
		}

		// Add JSON parsing validator
		download->addValidator(new JsonParseValidator(m_target));

		m_netTask->addNetAction(download);
		m_netTask->setAskRetry(false);

		// Connect signals
		connect(m_netTask.get(), &Task::succeeded, this, [this]() { finalizeLoad(MetaEntity::State::Synchronized); });
		connect(m_netTask.get(), &Task::failed, this, &EntityLoader::handleRemoteFailure);
		connect(m_netTask.get(), &Task::progress, this, &Task::setProgress);
		connect(m_netTask.get(), &Task::stepProgress, this, &EntityLoader::propagateStepProgress);
		connect(m_netTask.get(), &Task::status, this, &Task::setStatus);
		connect(m_netTask.get(), &Task::details, this, &Task::setDetails);

		m_netTask->start();
	}

	bool EntityLoader::loadCachedMetadata(const QString& cachePath, QString* errorOut)
	{
		try
		{
			setStatus(tr("Loading cached metadata"));

			QByteArray content		 = FS::read(cachePath);
			m_target->m_actualSha256 = Hashing::hash(content, Hashing::Algorithm::Sha256);

			if (m_target->m_state == MetaEntity::State::Pending)
			{
				QJsonDocument doc = Json::requireDocument(content, cachePath);
				QJsonObject root  = Json::requireObject(doc, cachePath);
				m_target->loadFromJson(root);
			}

			return true;
		}
		catch (const Exception& ex)
		{
			if (errorOut)
				*errorOut = ex.cause();
			return false;
		}
	}

	void EntityLoader::handleRemoteFailure(const QString& reason)
	{
		if (!m_canUseLocalFallback)
		{
			emitFailed(reason);
			return;
		}

		QString cachePath = QDir("meta").absoluteFilePath(m_target->cacheFilePath());
		QString cacheError;
		if (QFile::exists(cachePath) && loadCachedMetadata(cachePath, &cacheError))
		{
			m_target->m_state = MetaEntity::State::Cached;
			emitSucceeded();
			qWarning() << "Remote metadata refresh failed for" << m_target->cacheFilePath()
					   << "- falling back to cached metadata:" << reason;
			return;
		}

		if (!cacheError.isEmpty())
			qWarning() << "Cached fallback also failed for" << cachePath << ":" << cacheError;

		emitFailed(reason);
	}

	void EntityLoader::finalizeLoad(MetaEntity::State newState)
	{
		m_target->m_state = newState;
		if (newState == MetaEntity::State::Synchronized)
		{
			m_target->m_actualSha256 = m_target->m_expectedSha256;
		}
		emitSucceeded();
	}

	bool EntityLoader::canAbort() const
	{
		return m_netTask ? m_netTask->canAbort() : false;
	}

	bool EntityLoader::abort()
	{
		if (m_netTask)
		{
			Task::abort();
			return m_netTask->abort();
		}
		return Task::abort();
	}

} // namespace projt::meta
