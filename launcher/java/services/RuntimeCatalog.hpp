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
#include <QObject>

#include "BaseVersionList.h"
#include "QObjectPtr.h"
#include "java/core/RuntimeInstall.hpp"
#include "java/services/RuntimeProbeTask.hpp"
#include "tasks/Task.h"

namespace projt::java
{
	class RuntimeCatalogTask;

	class RuntimeCatalog : public BaseVersionList
	{
		Q_OBJECT
		enum class State
		{
			Idle,
			Loading,
			Ready
		};

	  public:
		enum class Scope
		{
			All,
			ManagedOnly
		};

		explicit RuntimeCatalog(QObject* parent = nullptr, Scope scope = Scope::All);

		Task::Ptr getLoadTask() override;
		bool isLoaded() override;
		const BaseVersion::Ptr at(int i) const override;
		int count() const override;
		void sortVersions() override;

		QVariant data(const QModelIndex& index, int role) const override;
		RoleList providesRoles() const override;

	  public slots:
		void updateListData(QList<BaseVersion::Ptr> versions) override;

	  private:
		void load();
		Task::Ptr currentTask() const;

		State m_state = State::Idle;
		shared_qobject_ptr<RuntimeCatalogTask> m_task;
		QList<BaseVersion::Ptr> m_entries;
		Scope m_scope;
	};

	class RuntimeCatalogTask : public Task
	{
		Q_OBJECT

	  public:
		explicit RuntimeCatalogTask(RuntimeCatalog* catalog, RuntimeCatalog::Scope scope);
		~RuntimeCatalogTask() override = default;

	  protected:
		void executeTask() override;

	  public slots:
		void probeFinished();

	  private:
		Task::Ptr m_job;
		RuntimeCatalog* m_catalog = nullptr;
		QList<RuntimeProbeTask::ProbeReport> m_results;
		RuntimeCatalog::Scope m_scope;
	};
} // namespace projt::java
