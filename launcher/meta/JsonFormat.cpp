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

#include "JsonFormat.hpp"

#include <QJsonArray>
#include <QDateTime>

#include "Index.hpp"
#include "Json.h"
#include "Version.hpp"
#include "VersionList.hpp"
#include "minecraft/OneSixVersionFormat.h"

namespace projt::meta
{

	SchemaVersion currentSchema()
	{
		return SchemaVersion::V1;
	}

	SchemaVersion detectSchemaVersion(const QJsonObject& root, bool required)
	{
		if (!root.contains("formatVersion"))
			return required ? SchemaVersion::Unknown : SchemaVersion::V1;

		QJsonValue val = root.value("formatVersion");
		if (!val.isDouble())
			return SchemaVersion::Unknown;

		int ver = val.toInt();
		return (ver == 0 || ver == 1) ? SchemaVersion::V1 : SchemaVersion::Unknown;
	}

	void writeSchemaVersion(QJsonObject& target, SchemaVersion version)
	{
		if (version != SchemaVersion::Unknown)
			target.insert("formatVersion", static_cast<int>(version));
	}

	DependencySet parseDependencies(const QJsonObject& source, const QString& fieldName)
	{
		DependencySet result;

		if (!source.contains(fieldName))
			return result;

		QJsonArray arr = Json::requireArray(source, fieldName);
		for (const QJsonValue& item : arr)
		{
			QJsonObject depObj = Json::requireObject(item);

			ComponentDependency dep;
			dep.uid			  = Json::requireString(depObj, "uid");
			dep.equalsVersion = Json::ensureString(depObj, "equals", QString());
			dep.suggests	  = Json::ensureString(depObj, "suggests", QString());

			result.insert(dep);
		}

		return result;
	}

	void writeDependencies(QJsonObject& target, const DependencySet& deps, const QString& fieldName)
	{
		if (deps.empty())
			return;

		QJsonArray arr;
		for (const auto& dep : deps)
		{
			QJsonObject obj;
			obj.insert("uid", dep.uid);

			if (!dep.equalsVersion.isEmpty())
				obj.insert("equals", dep.equalsVersion);

			if (!dep.suggests.isEmpty())
				obj.insert("suggests", dep.suggests);

			arr.append(obj);
		}

		target.insert(fieldName, arr);
	}

	namespace
	{

		MetaVersionList::Ptr buildVersionListFromPackage(const QJsonObject& pkg)
		{
			QString uid = Json::requireString(pkg, "uid");
			auto list	= std::make_shared<MetaVersionList>(uid);

			list->setDisplayName(Json::ensureString(pkg, "name", QString()));
			list->setExpectedChecksum(Json::ensureString(pkg, "sha256", QString()));

			return list;
		}

		MetaVersion::Ptr buildVersionFromJson(const QString& componentUid, const QJsonObject& obj, bool markStability)
		{
			QString verId = Json::requireString(obj, "version");
			auto ver	  = std::make_shared<MetaVersion>(componentUid, verId);

			QString timeStr = Json::requireString(obj, "releaseTime");
			QDateTime dt	= QDateTime::fromString(timeStr, Qt::ISODate);
			ver->setReleaseTimestamp(dt.toMSecsSinceEpoch() / 1000);

			ver->setReleaseType(Json::ensureString(obj, "type", QString()));
			ver->setStable(Json::ensureBoolean(obj, QStringLiteral("recommended"), false));
			ver->setVolatile(Json::ensureBoolean(obj, QStringLiteral("volatile"), false));

			DependencySet deps		= parseDependencies(obj, "requires");
			DependencySet conflicts = parseDependencies(obj, "conflicts");
			ver->setDependencies(deps, conflicts);

			QString sha = Json::ensureString(obj, "sha256", QString());
			if (!sha.isEmpty())
				ver->setExpectedChecksum(sha);

			if (markStability)
				ver->markAsStableCandidate();

			return ver;
		}

		MetaVersion::Ptr buildFullVersionFromJson(const QJsonObject& obj)
		{
			QString uid = Json::requireString(obj, "uid");
			auto ver	= buildVersionFromJson(uid, obj, false);

			VersionFilePtr data =
				OneSixVersionFormat::versionFileFromJson(QJsonDocument(obj),
														 QString("%1/%2.json").arg(uid, ver->versionId()),
														 obj.contains("order"));
			ver->setDetailedData(data);

			return ver;
		}

		MetaVersionList::Ptr buildVersionListFromPackageJson(const QJsonObject& obj)
		{
			QString uid = Json::requireString(obj, "uid");

			QList<QJsonObject> versionObjects = Json::requireIsArrayOf<QJsonObject>(obj, "versions");
			QList<MetaVersion::Ptr> versions;
			versions.reserve(versionObjects.size());

			for (const QJsonObject& vObj : versionObjects)
				versions.append(buildVersionFromJson(uid, vObj, true));

			auto list = std::make_shared<MetaVersionList>(uid);
			list->setDisplayName(Json::ensureString(obj, "name", QString()));
			list->setVersionEntries(versions);

			return list;
		}

		std::shared_ptr<MetaIndex> buildIndexFromJson(const QJsonObject& obj)
		{
			QList<QJsonObject> packages = Json::requireIsArrayOf<QJsonObject>(obj, "packages");
			QList<MetaVersionList::Ptr> components;
			components.reserve(packages.size());

			for (const QJsonObject& pkg : packages)
				components.append(buildVersionListFromPackage(pkg));

			return std::make_shared<MetaIndex>(components);
		}

	} // anonymous namespace

	void loadIndexFromJson(const QJsonObject& json, MetaIndex* index)
	{
		SchemaVersion schema = detectSchemaVersion(json);

		if (schema == SchemaVersion::V1)
			index->mergeWith(buildIndexFromJson(json));
		else
			throw MetaParseError(QObject::tr("Unsupported metadata schema version"));
	}

	void loadVersionListFromJson(const QJsonObject& json, MetaVersionList* list)
	{
		SchemaVersion schema = detectSchemaVersion(json);

		if (schema == SchemaVersion::V1)
			list->mergeWith(buildVersionListFromPackageJson(json));
		else
			throw MetaParseError(QObject::tr("Unsupported metadata schema version"));
	}

	void loadVersionFromJson(const QJsonObject& json, MetaVersion* version)
	{
		SchemaVersion schema = detectSchemaVersion(json);

		if (schema == SchemaVersion::V1)
			version->updateFrom(buildFullVersionFromJson(json));
		else
			throw MetaParseError(QObject::tr("Unsupported metadata schema version"));
	}

} // namespace projt::meta
