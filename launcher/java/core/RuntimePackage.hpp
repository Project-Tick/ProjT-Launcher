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

#include <QDateTime>
#include <QJsonObject>
#include <QString>

#include <memory>

#include "BaseVersion.h"
#include "java/core/RuntimeVersion.hpp"

namespace projt::java
{
	enum class PackageKind
	{
		Manifest,
		Archive,
		Unknown
	};

	class RuntimePackage : public BaseVersion
	{
	  public:
		QString descriptor() const override
		{
			return version.toString();
		}

		QString name() const override
		{
			return displayName;
		}

		QString typeString() const override
		{
			return vendor;
		}

		bool operator<(BaseVersion& a) const override;
		bool operator>(BaseVersion& a) const override;
		bool operator<(const RuntimePackage& rhs) const;
		bool operator==(const RuntimePackage& rhs) const;
		bool operator>(const RuntimePackage& rhs) const;

		QString displayName;
		QString vendor;
		QString url;
		QDateTime releaseTime;
		QString checksumType;
		QString checksumHash;
		PackageKind downloadKind = PackageKind::Unknown;
		QString packageType;
		RuntimeVersion version;
		QString runtimeOS;
	};

	using RuntimePackagePtr = std::shared_ptr<RuntimePackage>;

	PackageKind parsePackageKind(const QString& raw);
	QString packageKindToString(PackageKind kind);
	RuntimePackagePtr parseRuntimePackage(const QJsonObject& obj);
} // namespace projt::java