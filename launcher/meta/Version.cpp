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

#include "Version.hpp"

#include <QDateTime>
#include <algorithm>

#include "JsonFormat.hpp"

namespace projt::meta
{

	MetaVersion::MetaVersion(const QString& componentUid, const QString& versionId)
		: m_componentUid(componentUid),
		  m_versionId(versionId)
	{}

	QString MetaVersion::name() const
	{
		if (m_detailedData)
			return m_detailedData->name;
		return m_componentUid;
	}

	QDateTime MetaVersion::releaseTime() const
	{
		return QDateTime::fromMSecsSinceEpoch(m_timestamp * 1000, Qt::UTC);
	}

	QString MetaVersion::parentComponentVersion() const
	{
		auto mcDep = std::find_if(m_dependencies.begin(),
								  m_dependencies.end(),
								  [](const ComponentDependency& d)
								  { return d.uid == "net.minecraft" && !d.equalsVersion.isEmpty(); });

		if (mcDep != m_dependencies.end())
			return mcDep->equalsVersion;

		auto anyDep = std::find_if(m_dependencies.begin(),
								   m_dependencies.end(),
								   [](const ComponentDependency& d) { return !d.equalsVersion.isEmpty(); });

		if (anyDep != m_dependencies.end())
			return anyDep->equalsVersion;

		return QString();
	}

	QString MetaVersion::cacheFilePath() const
	{
		return m_componentUid + "/" + m_versionId + ".json";
	}

	void MetaVersion::loadFromJson(const QJsonObject& json)
	{
		loadVersionFromJson(json, this);
	}

	void MetaVersion::updateMetadataFrom(const MetaVersion::Ptr& source)
	{
		if (source->m_providesStability && m_stable != source->m_stable)
			setStable(source->m_stable);

		if (m_releaseType != source->m_releaseType)
			setReleaseType(source->m_releaseType);

		if (m_timestamp != source->m_timestamp)
			setReleaseTimestamp(source->m_timestamp);

		if (m_dependencies != source->m_dependencies || m_conflicts != source->m_conflicts)
		{
			m_dependencies = source->m_dependencies;
			m_conflicts	   = source->m_conflicts;
			emit dependenciesChanged();
		}

		if (m_volatile != source->m_volatile)
			setVolatile(source->m_volatile);

		if (!source->m_expectedSha256.isEmpty())
			m_expectedSha256 = source->m_expectedSha256;
	}

	void MetaVersion::updateFrom(const MetaVersion::Ptr& source)
	{
		updateMetadataFrom(source);

		if (source->m_detailedData)
			setDetailedData(source->m_detailedData);
	}

	void MetaVersion::setReleaseType(const QString& type)
	{
		m_releaseType = type;
		emit releaseTypeChanged();
	}

	void MetaVersion::setReleaseTimestamp(qint64 ts)
	{
		m_timestamp = ts;
		emit timestampChanged();
	}

	void MetaVersion::setDependencies(const DependencySet& deps, const DependencySet& conflicts)
	{
		m_dependencies = deps;
		m_conflicts	   = conflicts;
		emit dependenciesChanged();
	}

	void MetaVersion::setStable(bool stable)
	{
		m_stable = stable;
	}

	void MetaVersion::setVolatile(bool vol)
	{
		m_volatile = vol;
	}

	void MetaVersion::setDetailedData(const VersionFilePtr& data)
	{
		m_detailedData = data;
	}

	void MetaVersion::markAsStableCandidate()
	{
		m_providesStability = true;
	}

} // namespace projt::meta
