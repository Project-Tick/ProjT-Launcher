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

#include "VersionList.hpp"

#include <QDateTime>
#include <QEventLoop>
#include <algorithm>

#include "Application.h"
#include "Index.hpp"
#include "JsonFormat.hpp"
#include "net/Mode.h"
#include "tasks/SequentialTask.h"

namespace projt::meta
{

	MetaVersionList::MetaVersionList(const QString& componentUid, QObject* parent)
		: BaseVersionList(parent),
		  m_uid(componentUid)
	{
		setObjectName("MetaVersionList: " + componentUid);

		m_activeRoles = { VersionPointerRole, VersionRole,		VersionIdRole, ParentVersionRole,
						  TypeRole,			  ComponentUidRole, TimestampRole, DependencyRole,
						  SortRole,			  RecommendedRole,	LatestRole,	   VersionPtrRole };
	}

	bool MetaVersionList::isLoaded()
	{
		return MetaEntity::isReady();
	}

	Task::Ptr MetaVersionList::getLoadTask()
	{
		auto seq = makeShared<SequentialTask>(tr("Load metadata for %1").arg(m_uid));
		seq->addTask(APPLICATION->metadataIndex()->createLoadTask(Net::Mode::Online));
		seq->addTask(this->createLoadTask(Net::Mode::Online));
		return seq;
	}

	const BaseVersion::Ptr MetaVersionList::at(int i) const
	{
		return m_entries.at(i);
	}

	int MetaVersionList::count() const
	{
		return m_entries.size();
	}

	void MetaVersionList::sortVersions()
	{
		beginResetModel();
		std::sort(m_entries.begin(),
				  m_entries.end(),
				  [](const MetaVersion::Ptr& a, const MetaVersion::Ptr& b)
				  { return a->asComparableVersion() < b->asComparableVersion(); });
		endResetModel();
	}

	QVariant MetaVersionList::data(const QModelIndex& idx, int role) const
	{
		if (!idx.isValid() || idx.row() < 0 || idx.row() >= m_entries.size())
			return QVariant();

		const MetaVersion::Ptr& ver = m_entries.at(idx.row());

		switch (role)
		{
			case VersionPointerRole: return QVariant::fromValue(std::dynamic_pointer_cast<BaseVersion>(ver));
			case VersionRole:
			case VersionIdRole: return ver->versionId();
			case ParentVersionRole:
			{
				QString parent = ver->parentComponentVersion();
				return parent.isEmpty() ? QVariant() : parent;
			}
			case TypeRole: return ver->releaseType();
			case ComponentUidRole: return ver->componentUid();
			case TimestampRole: return ver->releaseTime();
			case DependencyRole: return QVariant::fromValue(ver->dependencies());
			case SortRole: return ver->releaseTimestamp();
			case VersionPtrRole: return QVariant::fromValue(ver);
			case RecommendedRole: return ver->isStable() || m_externalStableVersions.contains(ver->versionId());
			case JavaMajorRole:
			{
				QString display = ver->versionId();
				if (display.startsWith("java"))
					display = "Java " + display.mid(4);
				return display;
			}
			default: return QVariant();
		}
	}

	BaseVersionList::RoleList MetaVersionList::providesRoles() const
	{
		return m_activeRoles;
	}

	void MetaVersionList::setAvailableRoles(RoleList roles)
	{
		m_activeRoles = roles;
	}

	QHash<int, QByteArray> MetaVersionList::roleNames() const
	{
		QHash<int, QByteArray> names = BaseVersionList::roleNames();
		names.insert(ComponentUidRole, "componentUid");
		names.insert(TimestampRole, "timestamp");
		names.insert(SortRole, "sortKey");
		names.insert(DependencyRole, "dependencies");
		return names;
	}

	QString MetaVersionList::cacheFilePath() const
	{
		return m_uid + "/index.json";
	}

	void MetaVersionList::loadFromJson(const QJsonObject& json)
	{
		loadVersionListFromJson(json, this);
	}

	MetaVersion::Ptr MetaVersionList::getMetaVersion(const QString& versionId)
	{
		return m_versionIndex.value(versionId, nullptr);
	}

	MetaVersion::Ptr MetaVersionList::getOrCreateVersion(const QString& versionId)
	{
		MetaVersion::Ptr existing = m_versionIndex.value(versionId);
		if (existing)
			return existing;

		auto newVer = std::make_shared<MetaVersion>(m_uid, versionId);
		int row		= m_entries.size();
		beginInsertRows(QModelIndex(), row, row);
		m_versionIndex.insert(versionId, newVer);
		registerVersion(row, newVer);
		m_entries.append(newVer);
		endInsertRows();

		return newVer;
	}

	bool MetaVersionList::containsVersion(const QString& versionId) const
	{
		return m_versionIndex.contains(versionId);
	}

	void MetaVersionList::setDisplayName(const QString& name)
	{
		m_displayName = name;
		emit displayNameChanged(name);
	}

	void MetaVersionList::setVersionEntries(const QList<MetaVersion::Ptr>& entries)
	{
		beginResetModel();

		m_entries = entries;

		std::sort(m_entries.begin(),
				  m_entries.end(),
				  [](const MetaVersion::Ptr& a, const MetaVersion::Ptr& b)
				  { return a->releaseTimestamp() > b->releaseTimestamp(); });

		m_versionIndex.clear();
		for (int i = 0; i < m_entries.size(); ++i)
		{
			const auto& ver = m_entries.at(i);
			m_versionIndex.insert(ver->versionId(), ver);
			registerVersion(i, ver);
		}

		auto stableIt = std::find_if(m_entries.constBegin(),
									 m_entries.constEnd(),
									 [](const MetaVersion::Ptr& v) { return v->releaseType() == "release"; });

		m_stableVersion = (stableIt != m_entries.constEnd()) ? *stableIt : nullptr;

		endResetModel();
	}

	void MetaVersionList::addExternalStableVersions(const QStringList& versions)
	{
		m_externalStableVersions.append(versions);
	}

	void MetaVersionList::clearExternalStableVersions()
	{
		m_externalStableVersions.clear();
	}

	const MetaVersion::Ptr& MetaVersionList::selectBetterVersion(const MetaVersion::Ptr& a, const MetaVersion::Ptr& b)
	{
		if (!a)
			return b;
		if (!b)
			return a;

		if (a->releaseType() != b->releaseType())
		{
			if (a->releaseType() == "release")
				return a;
			if (b->releaseType() == "release")
				return b;
		}

		return (a->releaseTimestamp() > b->releaseTimestamp()) ? a : b;
	}

	void MetaVersionList::updateFromIndex(const MetaVersionList::Ptr& other)
	{
		if (m_displayName != other->m_displayName)
			setDisplayName(other->m_displayName);

		if (!other->m_expectedSha256.isEmpty())
			m_expectedSha256 = other->m_expectedSha256;
	}

	void MetaVersionList::mergeWith(const MetaVersionList::Ptr& other)
	{
		if (m_displayName != other->m_displayName)
			setDisplayName(other->m_displayName);

		if (!other->m_expectedSha256.isEmpty())
			m_expectedSha256 = other->m_expectedSha256;

		beginResetModel();

		for (const auto& incoming : other->m_entries)
		{
			MetaVersion::Ptr existing = m_versionIndex.value(incoming->versionId());

			if (existing)
			{
				existing->updateMetadataFrom(incoming);
			}
			else
			{
				m_versionIndex.insert(incoming->versionId(), incoming);
				int row = m_entries.size();
				registerVersion(row, incoming);
				m_entries.append(incoming);
			}

			m_stableVersion = selectBetterVersion(m_stableVersion, incoming);
		}

		endResetModel();
	}

	void MetaVersionList::registerVersion(int row, const MetaVersion::Ptr& version)
	{
		disconnect(version.get(), &MetaVersion::dependenciesChanged, this, nullptr);
		disconnect(version.get(), &MetaVersion::timestampChanged, this, nullptr);
		disconnect(version.get(), &MetaVersion::releaseTypeChanged, this, nullptr);

		connect(version.get(),
				&MetaVersion::dependenciesChanged,
				this,
				[this, row]() { emit dataChanged(index(row), index(row), { DependencyRole }); });
		connect(version.get(),
				&MetaVersion::timestampChanged,
				this,
				[this, row]() { emit dataChanged(index(row), index(row), { TimestampRole, SortRole }); });
		connect(version.get(),
				&MetaVersion::releaseTypeChanged,
				this,
				[this, row]() { emit dataChanged(index(row), index(row), { TypeRole }); });
	}

	BaseVersion::Ptr MetaVersionList::getRecommended() const
	{
		return m_stableVersion;
	}

	void MetaVersionList::waitUntilReady()
	{
		if (isLoaded())
			return;

		QEventLoop loop;
		auto task = getLoadTask();
		connect(task.get(), &Task::finished, &loop, &QEventLoop::quit);
		task->start();
		loop.exec();
	}

	MetaVersion::Ptr MetaVersionList::stableForParent(const QString& parentUid, const QString& parentVersion)
	{
		for (const auto& ver : m_entries)
		{
			if (!ver->isStable())
				continue;

			const auto& deps = ver->dependencies();
			auto match		 = std::find_if(deps.begin(),
										deps.end(),
										[&](const ComponentDependency& d)
										{ return d.uid == parentUid && d.equalsVersion == parentVersion; });

			if (match != deps.end())
				return ver;
		}
		return nullptr;
	}

	MetaVersion::Ptr MetaVersionList::latestForParent(const QString& parentUid, const QString& parentVersion)
	{
		MetaVersion::Ptr best = nullptr;

		for (const auto& ver : m_entries)
		{
			const auto& deps = ver->dependencies();
			auto match		 = std::find_if(deps.begin(),
										deps.end(),
										[&](const ComponentDependency& d)
										{ return d.uid == parentUid && d.equalsVersion == parentVersion; });

			if (match != deps.end())
				best = selectBetterVersion(best, ver);
		}

		return best;
	}

} // namespace projt::meta
