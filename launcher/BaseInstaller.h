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

#include <memory>

#include "BaseVersion.h"

class MinecraftInstance;
class QDir;
class QString;
class QObject;
class Task;
class BaseVersion;

class BaseInstaller
{
  public:
	BaseInstaller();
	virtual ~BaseInstaller() {};
	bool isApplied(MinecraftInstance* on);

	virtual bool add(MinecraftInstance* to);
	virtual bool remove(MinecraftInstance* from);

	virtual Task* createInstallTask(MinecraftInstance* instance, BaseVersion::Ptr version, QObject* parent) = 0;

  protected:
	virtual QString id() const = 0;
	QString filename(const QString& root) const;
	QDir patchesDir(const QString& root) const;
};
