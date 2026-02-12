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

#include <QHash>
#include <QJsonObject>
#include <QStringList>
#include <memory>

#include "BaseEntity.hpp"
#include "BaseVersionList.h"
#include "Version.hpp"

namespace projt::meta
{

	/**
	 * @brief Collection of MetaVersion entries for a single component.
	 */
	class MetaVersionList : public BaseVersionList, public MetaEntity
	{
		Q_OBJECT
		Q_PROPERTY(QString uid READ uid CONSTANT)
		Q_PROPERTY(QString displayName READ displayName NOTIFY displayNameChanged)

	  public:
		using Ptr = std::shared_ptr<MetaVersionList>;

		explicit MetaVersionList(const QString& componentUid, QObject* parent = nullptr);
		~MetaVersionList() override = default;

		enum Roles
		{
			ComponentUidRole = Qt::UserRole + 200,
			TimestampRole,
			DependencyRole,
			VersionPtrRole
		};

		// BaseVersionList interface
		bool isLoaded() override;
		Task::Ptr getLoadTask() override;
		const BaseVersion::Ptr at(int i) const override;
		int count() const override;
		void sortVersions() override;
		BaseVersion::Ptr getRecommended() const override;
		QVariant data(const QModelIndex& index, int role) const override;
		RoleList providesRoles() const override;
		QHash<int, QByteArray> roleNames() const override;

		// MetaEntity interface
		QString cacheFilePath() const override;
		void loadFromJson(const QJsonObject& json) override;

		// Identity
		QString uid() const
		{
			return m_uid;
		}
		QString displayName() const
		{
			return m_displayName.isEmpty() ? m_uid : m_displayName;
		}

		// Version access
		MetaVersion::Ptr getMetaVersion(const QString& versionId);
		MetaVersion::Ptr getOrCreateVersion(const QString& versionId);
		bool containsVersion(const QString& versionId) const;
		QList<MetaVersion::Ptr> allVersions() const
		{
			return m_entries;
		}

		// Filtered lookups
		MetaVersion::Ptr stableForParent(const QString& parentUid, const QString& parentVersion);
		MetaVersion::Ptr latestForParent(const QString& parentUid, const QString& parentVersion);

		// External stable versions
		void addExternalStableVersions(const QStringList& versions);
		void clearExternalStableVersions();

		// Blocking load
		void waitUntilReady();

		// Configuration
		void setAvailableRoles(RoleList roles);

	  public: // For parsers
		void setDisplayName(const QString& name);
		void setVersionEntries(const QList<MetaVersion::Ptr>& entries);
		void mergeWith(const MetaVersionList::Ptr& other);
		void updateFromIndex(const MetaVersionList::Ptr& other);

	  signals:
		void displayNameChanged(const QString& name);

	  protected slots:
		void updateListData(QList<BaseVersion::Ptr>) override
		{}

	  private:
		void registerVersion(int row, const MetaVersion::Ptr& version);
		static const MetaVersion::Ptr& selectBetterVersion(const MetaVersion::Ptr& a, const MetaVersion::Ptr& b);

		QString m_uid;
		QString m_displayName;

		QList<MetaVersion::Ptr> m_entries;
		QHash<QString, MetaVersion::Ptr> m_versionIndex;
		QStringList m_externalStableVersions;

		MetaVersion::Ptr m_stableVersion;
		RoleList m_activeRoles;
	};

} // namespace projt::meta

Q_DECLARE_METATYPE(projt::meta::MetaVersionList::Ptr)
