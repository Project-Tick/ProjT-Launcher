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
 * ======================================================================== */

#pragma once

#include <QDebug>
#include <QObject>

#include "minecraft/mod/ShaderPack.hpp"

#include "tasks/Task.h"

namespace ShaderPackUtils
{

	enum class ProcessingLevel
	{
		Full,
		BasicInfoOnly
	};

	bool process(ShaderPack& pack, ProcessingLevel level = ProcessingLevel::Full);

	bool processZIP(ShaderPack& pack, ProcessingLevel level = ProcessingLevel::Full);
	bool processFolder(ShaderPack& pack, ProcessingLevel level = ProcessingLevel::Full);

	/** Checks whether a file is valid as a shader pack or not. */
	bool validate(QFileInfo file);
} // namespace ShaderPackUtils

class LocalShaderPackParseTask : public Task
{
	Q_OBJECT
  public:
	LocalShaderPackParseTask(int token, ShaderPack& sp);

	bool canAbort() const override
	{
		return true;
	}
	bool abort() override;

	void executeTask() override;

	int token() const
	{
		return m_token;
	}

  private:
	int m_token;

	ShaderPack& m_shader_pack;

	bool m_aborted = false;
};
