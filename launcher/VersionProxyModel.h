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
#include <QAbstractProxyModel>
#include "BaseVersionList.h"

#include <Filter.h>

class VersionFilterModel;

class VersionProxyModel : public QAbstractProxyModel
{
	Q_OBJECT
  public:
	enum Column
	{
		Name,
		ParentVersion,
		Branch,
		Type,
		CPUArchitecture,
		Path,
		Time,
		JavaName,
		JavaMajor
	};
	using FilterMap = QHash<BaseVersionList::ModelRoles, Filter>;

  public:
	VersionProxyModel(QObject* parent = 0);
	virtual ~VersionProxyModel() {};

	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	virtual QModelIndex mapFromSource(const QModelIndex& sourceIndex) const override;
	virtual QModelIndex mapToSource(const QModelIndex& proxyIndex) const override;
	virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	virtual QModelIndex parent(const QModelIndex& child) const override;
	virtual void setSourceModel(QAbstractItemModel* sourceModel) override;

	const FilterMap& filters() const;
	const QString& search() const;
	void setFilter(BaseVersionList::ModelRoles column, Filter filter);
	void setSearch(const QString& search);
	void clearFilters();
	QModelIndex getRecommended() const;
	QModelIndex getVersion(const QString& version) const;
	void setCurrentVersion(const QString& version);
  private slots:

	void sourceDataChanged(const QModelIndex& source_top_left, const QModelIndex& source_bottom_right);

	void sourceAboutToBeReset();
	void sourceReset();

	void sourceRowsAboutToBeInserted(const QModelIndex& parent, int first, int last);
	void sourceRowsInserted(const QModelIndex& parent, int first, int last);

	void sourceRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last);
	void sourceRowsRemoved(const QModelIndex& parent, int first, int last);

	void sourceRowsAboutToBeMoved(const QModelIndex& sourceParent,
								  int sourceStart,
								  int sourceEnd,
								  const QModelIndex& destinationParent,
								  int destinationRow);
	void sourceRowsMoved(const QModelIndex& sourceParent,
						 int sourceStart,
						 int sourceEnd,
						 const QModelIndex& destinationParent,
						 int destinationRow);

	void sourceLayoutAboutToBeChanged(const QList<QPersistentModelIndex>& sourceParents,
									  QAbstractItemModel::LayoutChangeHint hint);
	void sourceLayoutChanged(const QList<QPersistentModelIndex>& sourceParents,
							 QAbstractItemModel::LayoutChangeHint hint);

  private:
	QList<Column> m_columns;
	FilterMap m_filters;
	QString m_search;
	BaseVersionList::RoleList roles;
	VersionFilterModel* filterModel;
	bool hasRecommended = false;
	bool hasLatest		= false;
	QString m_currentVersion;
	QString m_parentVersionName = tr("Minecraft"); // Default, updated from metadata
};
