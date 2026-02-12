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

#include <QAbstractListModel>
#include <QHash>

#include "BaseEntity.hpp"
#include "VersionList.hpp"
#include "net/Mode.h"

class Task;

namespace projt::meta
{

	/**
	 * @brief Root index of all available metadata components.
	 */
	class MetaIndex : public QAbstractListModel, public MetaEntity
	{
		Q_OBJECT

	  public:
		explicit MetaIndex(QObject* parent = nullptr);
		explicit MetaIndex(const QList<MetaVersionList::Ptr>& components, QObject* parent = nullptr);
		~MetaIndex() override = default;

		enum
		{
			ComponentUidRole = Qt::UserRole,
			ComponentNameRole,
			ComponentListRole
		};

		// QAbstractListModel interface
		QVariant data(const QModelIndex& index, int role) const override;
		int rowCount(const QModelIndex& parent = {}) const override;
		int columnCount(const QModelIndex& parent = {}) const override;
		QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

		// MetaEntity interface
		QString cacheFilePath() const override
		{
			return QStringLiteral("index.json");
		}
		void loadFromJson(const QJsonObject& json) override;

		// Component access
		bool hasComponent(const QString& uid) const;
		MetaVersionList::Ptr component(const QString& uid);
		MetaVersion::Ptr version(const QString& componentUid, const QString& versionId);
		QList<MetaVersionList::Ptr> allComponents() const
		{
			return m_components;
		}

		// Load operations
		Task::Ptr loadVersionTask(const QString& componentUid,
								  const QString& versionId = {},
								  Net::Mode mode		   = Net::Mode::Online,
								  bool forceRefresh		   = false);

		// Blocking load
		MetaVersion::Ptr loadVersionBlocking(const QString& componentUid, const QString& versionId);

	  public: // For parsers
		void mergeWith(const std::shared_ptr<MetaIndex>& other);

	  private:
		void bindComponentSignals(int row, const MetaVersionList::Ptr& component);

		QList<MetaVersionList::Ptr> m_components;
		QHash<QString, MetaVersionList::Ptr> m_componentIndex;
	};

} // namespace projt::meta
