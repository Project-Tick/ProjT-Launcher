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

#include "BaseVersion.h"
#include "java/core/RuntimeVersion.hpp"

#include <memory>
#include <utility>

namespace projt::java
{
	struct RuntimeInstall : public BaseVersion
	{
		RuntimeInstall() = default;
		RuntimeInstall(RuntimeVersion version, QString arch, QString path)
			: version(std::move(version)),
			  arch(std::move(arch)),
			  path(std::move(path))
		{}

		QString descriptor() const override
		{
			return version.toString();
		}
		QString name() const override
		{
			return version.toString();
		}
		QString typeString() const override
		{
			return arch;
		}

		bool operator<(BaseVersion& a) const override;
		bool operator>(BaseVersion& a) const override;
		bool operator<(const RuntimeInstall& rhs) const;
		bool operator==(const RuntimeInstall& rhs) const;
		bool operator>(const RuntimeInstall& rhs) const;

		RuntimeVersion version;
		QString arch;
		QString path;
		QString vendor;
		bool recommended = false;
		bool is_64bit	 = false;
		bool managed	 = false;
	};

	using RuntimeInstallPtr = std::shared_ptr<RuntimeInstall>;
} // namespace projt::java
