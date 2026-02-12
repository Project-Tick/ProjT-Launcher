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
 *
 * === Upstream License Block (Do Not Modify) ==============================
 *
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (C) 2022 Rachel Powers <508861+Ryex@users.noreply.github.com>
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
 *
 * This file incorporates work covered by the following copyright and
 * permission notice:
 *
 *      Copyright 2013-2021 MultiMC Contributors
 *
 *      Licensed under the Apache License, Version 2.0 (the "License");
 *      you may not use this file except in compliance with the License.
 *      You may obtain a copy of the License at
 *
 *          http://www.apache.org/licenses/LICENSE-2.0
 *
 *      Unless required by applicable law or agreed to in writing, software
 *      distributed under the License is distributed on an "AS IS" BASIS,
 *      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *      See the License for the specific language governing permissions and
 *      limitations under the License.
 *
 * ======================================================================== */

#pragma once

#include <QDebug>
#include <QObject>

#include "minecraft/mod/DataPack.hpp"

#include "tasks/Task.h"

namespace DataPackUtils
{

	enum class ProcessingLevel
	{
		Full,
		BasicInfoOnly
	};

	bool process(DataPack* pack, ProcessingLevel level = ProcessingLevel::Full);

	bool processZIP(DataPack* pack, ProcessingLevel level = ProcessingLevel::Full);
	bool processFolder(DataPack* pack, ProcessingLevel level = ProcessingLevel::Full);

	bool processMCMeta(DataPack* pack, QByteArray&& raw_data);

	QString processComponent(const QJsonValue& value, bool strikethrough = false, bool underline = false);

	bool processPackPNG(const DataPack* pack, QByteArray&& raw_data);

	/// processes ONLY the pack.png (rest of the pack may be invalid)
	bool processPackPNG(const DataPack* pack);

	/** Checks whether a file is valid as a data pack or not. */
	bool validate(QFileInfo file);

	/** Checks whether a file is valid as a resource pack or not. */
	bool validateResourcePack(QFileInfo file);

} // namespace DataPackUtils

class LocalDataPackParseTask : public Task
{
	Q_OBJECT
  public:
	LocalDataPackParseTask(int token, DataPack* dp);

	void executeTask() override;

	int token() const
	{
		return m_token;
	}

  private:
	int m_token;

	DataPack* m_data_pack;
};