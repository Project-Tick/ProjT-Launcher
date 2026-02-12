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
 * === Upstream License Block (Do Not Modify) ==============================
 * Copyright 2013-2021 MultiMC Contributors
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
 *
  ======================================================================== */
#pragma once

#include <QAbstractListModel>
#include <QObject>
#include <QVariant>

#include "BaseVersion.h"
#include "QObjectPtr.h"
#include "tasks/Task.h"

/*!
 * \brief Class that each instance type's version list derives from.
 * Version lists are the lists that keep track of the available game versions
 * for that instance. This list will not be loaded on startup. It will be loaded
 * when the list's load function is called. Before using the version list, you
 * should check to see if it has been loaded yet and if not, load the list.
 *
 * Note that this class also inherits from QAbstractListModel. Methods from that
 * class determine how this version list shows up in a list view. Said methods
 * all have a default implementation, but they can be overridden by plugins to
 * change the behavior of the list.
 */
class BaseVersionList : public QAbstractListModel
{
	Q_OBJECT
  public:
	enum ModelRoles
	{
		VersionPointerRole = Qt::UserRole,
		VersionRole,
		VersionIdRole,
		ParentVersionRole,
		RecommendedRole,
		LatestRole,
		TypeRole,
		BranchRole,
		PathRole,
		JavaNameRole,
		JavaMajorRole,
		CPUArchitectureRole,
		SortRole
	};
	using RoleList = QList<int>;

	explicit BaseVersionList(QObject* parent = 0);

	/*!
	 * \brief Gets a task that will reload the version list.
	 * Simply execute the task to load the list.
	 * The task returned by this function should reset the model when it's done.
	 * \return A pointer to a task that reloads the version list.
	 */
	virtual Task::Ptr getLoadTask() = 0;

	//! Checks whether or not the list is loaded. If this returns false, the list should be
	// loaded.
	virtual bool isLoaded() = 0;

	//! Gets the version at the given index.
	virtual const BaseVersion::Ptr at(int i) const = 0;

	//! Returns the number of versions in the list.
	virtual int count() const = 0;

	//////// List Model Functions ////////
	QVariant data(const QModelIndex& index, int role) const override;
	int rowCount(const QModelIndex& parent) const override;
	int columnCount(const QModelIndex& parent) const override;
	QHash<int, QByteArray> roleNames() const override;

	//! which roles are provided by this version list?
	virtual RoleList providesRoles() const;

	/*!
	 * \brief Finds a version by its descriptor.
	 * \param descriptor The descriptor of the version to find.
	 * \return A const pointer to the version with the given descriptor. NULL if
	 * one doesn't exist.
	 */
	virtual BaseVersion::Ptr findVersion(const QString& descriptor);

	/*!
	 * \brief Gets the recommended version from this list
	 * If the list doesn't support recommended versions, this works exactly as getLatestStable
	 */
	virtual BaseVersion::Ptr getRecommended() const;

	/*!
	 * Sorts the version list.
	 */
	virtual void sortVersions() = 0;

  protected slots:
	/*!
	 * Updates this list with the given list of versions.
	 * This is done by copying each version in the given list and inserting it
	 * into this one.
	 * We need to do this so that we can set the parents of the versions are set to this
	 * version list. This can't be done in the load task, because the versions the load
	 * task creates are on the load task's thread and Qt won't allow their parents
	 * to be set to something created on another thread.
	 * To get around that problem, we invoke this method on the GUI thread, which
	 * then copies the versions and sets their parents correctly.
	 * \param versions List of versions whose parents should be set.
	 */
	virtual void updateListData(QList<BaseVersion::Ptr> versions) = 0;
};
