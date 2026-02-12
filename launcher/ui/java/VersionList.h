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

#include "BaseVersionList.h"
#include "java/core/RuntimePackage.hpp"
#include "meta/Version.hpp"

namespace Java
{

	class VersionList : public BaseVersionList
	{
		Q_OBJECT

	  public:
		explicit VersionList(projt::meta::MetaVersion::Ptr m_version, QObject* parent = 0);

		Task::Ptr getLoadTask() override;
		bool isLoaded() override;
		const BaseVersion::Ptr at(int i) const override;
		int count() const override;
		void sortVersions() override;

		QVariant data(const QModelIndex& index, int role) const override;
		RoleList providesRoles() const override;

	  protected slots:
		void updateListData(QList<BaseVersion::Ptr>) override
		{}

	  protected:
		projt::meta::MetaVersion::Ptr m_version;
		QList<projt::java::RuntimePackagePtr> m_vlist;
	};

} // namespace Java
