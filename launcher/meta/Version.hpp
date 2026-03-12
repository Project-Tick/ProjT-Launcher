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

#include <QDateTime>
#include <QJsonObject>
#include <QObject>
#include <memory>

#include "BaseEntity.hpp"
#include "BaseVersion.h"
#include "JsonFormat.hpp"
#include "Version.h"
#include "minecraft/VersionFile.h"

namespace projt::meta
{

	/**
	 * @brief Single version entry in the metadata system.
	 */
	class MetaVersion : public QObject, public BaseVersion, public MetaEntity
	{
		Q_OBJECT

	  public:
		using Ptr = std::shared_ptr<MetaVersion>;

		MetaVersion(const QString& componentUid, const QString& versionId);
		~MetaVersion() override = default;

		// BaseVersion interface
		QString descriptor() const override
		{
			return m_versionId;
		}
		QString name() const override;
		QString typeString() const override
		{
			return m_releaseType;
		}

		// Identity
		QString componentUid() const
		{
			return m_componentUid;
		}
		QString versionId() const
		{
			return m_versionId;
		}

		// Release info
		QString releaseType() const
		{
			return m_releaseType;
		}
		QDateTime releaseTime() const;
		qint64 releaseTimestamp() const
		{
			return m_timestamp;
		}

		// Flags
		bool isStable() const
		{
			return m_stable;
		}
		bool isVolatile() const
		{
			return m_volatile;
		}

		// Dependencies
		const DependencySet& dependencies() const
		{
			return m_dependencies;
		}
		const DependencySet& conflicts() const
		{
			return m_conflicts;
		}
		QString parentComponentVersion() const;

		// Detailed data
		bool hasDetailedData() const
		{
			return m_detailedData != nullptr;
		}
		const VersionFilePtr& detailedData() const
		{
			return m_detailedData;
		}
		bool isFullyLoaded() const
		{
			return hasDetailedData() && isReady();
		}

		// MetaEntity interface
		QString cacheFilePath() const override;
		void loadFromJson(const QJsonObject& json) override;

		// Version comparison
		::Version asComparableVersion() const
		{
			return ::Version(m_versionId);
		}

		// Merge operations
		void updateFrom(const MetaVersion::Ptr& source);
		void updateMetadataFrom(const MetaVersion::Ptr& source);

	  public: // Setters for parsers
		void setReleaseType(const QString& type);
		void setReleaseTimestamp(qint64 ts);
		void setDependencies(const DependencySet& deps, const DependencySet& conflicts);
		void setStable(bool stable);
		void setVolatile(bool vol);
		void setDetailedData(const VersionFilePtr& data);
		void markAsStableCandidate();

	  signals:
		void releaseTypeChanged();
		void timestampChanged();
		void dependenciesChanged();

	  private:
		QString m_componentUid;
		QString m_versionId;
		QString m_releaseType;
		qint64 m_timestamp = 0;

		bool m_stable			 = false;
		bool m_volatile			 = false;
		bool m_providesStability = false;

		DependencySet m_dependencies;
		DependencySet m_conflicts;

		VersionFilePtr m_detailedData;
	};

} // namespace projt::meta

Q_DECLARE_METATYPE(projt::meta::MetaVersion::Ptr)
