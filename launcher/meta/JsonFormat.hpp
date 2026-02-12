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

#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <set>

#include "Exception.h"

namespace projt::meta
{

	class MetaIndex;
	class MetaVersion;
	class MetaVersionList;

	enum class SchemaVersion
	{
		Unknown = -1,
		V1		= 1
	};

	class MetaParseError : public Exception
	{
	  public:
		using Exception::Exception;
	};

	struct ComponentDependency
	{
		QString uid;
		QString equalsVersion;
		QString suggests;

		bool operator==(const ComponentDependency& other) const
		{
			return uid == other.uid;
		}
		bool operator<(const ComponentDependency& other) const
		{
			return uid < other.uid;
		}

		bool deepEquals(const ComponentDependency& other) const
		{
			return uid == other.uid && equalsVersion == other.equalsVersion && suggests == other.suggests;
		}
	};

	using DependencySet = std::set<ComponentDependency>;

	SchemaVersion detectSchemaVersion(const QJsonObject& root, bool required = true);
	void writeSchemaVersion(QJsonObject& target, SchemaVersion version);
	SchemaVersion currentSchema();

	DependencySet parseDependencies(const QJsonObject& source, const QString& fieldName = "requires");
	void writeDependencies(QJsonObject& target, const DependencySet& deps, const QString& fieldName = "requires");

	void loadIndexFromJson(const QJsonObject& json, MetaIndex* index);
	void loadVersionFromJson(const QJsonObject& json, MetaVersion* version);
	void loadVersionListFromJson(const QJsonObject& json, MetaVersionList* list);

} // namespace projt::meta

Q_DECLARE_METATYPE(std::set<projt::meta::ComponentDependency>)
