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

#include "java/core/RuntimePackage.hpp"

#include "Json.h"
#include "StringUtils.h"
#include "minecraft/ParseUtils.h"

#include <typeinfo>

namespace projt::java
{
	PackageKind parsePackageKind(const QString& raw)
	{
		if (raw == "manifest")
		{
			return PackageKind::Manifest;
		}
		if (raw == "archive")
		{
			return PackageKind::Archive;
		}
		return PackageKind::Unknown;
	}

	QString packageKindToString(PackageKind kind)
	{
		switch (kind)
		{
			case PackageKind::Manifest: return "manifest";
			case PackageKind::Archive: return "archive";
			case PackageKind::Unknown: break;
		}
		return "unknown";
	}

	RuntimePackagePtr parseRuntimePackage(const QJsonObject& in)
	{
		auto meta = std::make_shared<RuntimePackage>();

		meta->displayName  = Json::ensureString(in, "name", "");
		meta->vendor	   = Json::ensureString(in, "vendor", "");
		meta->url		   = Json::ensureString(in, "url", "");
		meta->releaseTime  = timeFromS3Time(Json::ensureString(in, "releaseTime", ""));
		meta->downloadKind = parsePackageKind(Json::ensureString(in, "downloadType", ""));
		meta->packageType  = Json::ensureString(in, "packageType", "");
		meta->runtimeOS	   = Json::ensureString(in, "runtimeOS", "unknown");

		if (in.contains("checksum"))
		{
			auto obj		   = Json::requireObject(in, "checksum");
			meta->checksumHash = Json::ensureString(obj, "hash", "");
			meta->checksumType = Json::ensureString(obj, "type", "");
		}

		if (in.contains("version"))
		{
			auto obj	  = Json::requireObject(in, "version");
			auto name	  = Json::ensureString(obj, "name", "");
			auto major	  = Json::ensureInteger(obj, "major", 0);
			auto minor	  = Json::ensureInteger(obj, "minor", 0);
			auto security = Json::ensureInteger(obj, "security", 0);
			auto build	  = Json::ensureInteger(obj, "build", 0);
			meta->version = RuntimeVersion(major, minor, security, build, name);
		}
		return meta;
	}

	bool RuntimePackage::operator<(const RuntimePackage& rhs) const
	{
		if (version < rhs.version)
		{
			return true;
		}
		if (version > rhs.version)
		{
			return false;
		}
		if (releaseTime < rhs.releaseTime)
		{
			return true;
		}
		if (releaseTime > rhs.releaseTime)
		{
			return false;
		}
		return StringUtils::naturalCompare(displayName, rhs.displayName, Qt::CaseInsensitive) < 0;
	}

	bool RuntimePackage::operator==(const RuntimePackage& rhs) const
	{
		return version == rhs.version && displayName == rhs.displayName;
	}

	bool RuntimePackage::operator>(const RuntimePackage& rhs) const
	{
		return (!operator<(rhs)) && (!operator==(rhs));
	}

	bool RuntimePackage::operator<(BaseVersion& a) const
	{
		try
		{
			return operator<(dynamic_cast<RuntimePackage&>(a));
		}
		catch (const std::bad_cast&)
		{
			return BaseVersion::operator<(a);
		}
	}

	bool RuntimePackage::operator>(BaseVersion& a) const
	{
		try
		{
			return operator>(dynamic_cast<RuntimePackage&>(a));
		}
		catch (const std::bad_cast&)
		{
			return BaseVersion::operator>(a);
		}
	}
} // namespace projt::java
