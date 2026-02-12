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
 *
 * === Upstream License Block (Do Not Modify) ==============================
 *
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (c) 2022 flowln <flowlnlnln@gmail.com>
 *  Copyright (C) 2022 Sefa Eyeoglu <contact@scrumplex.net>
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

#include "minecraft/mod/TexturePack.hpp"

#include "tasks/Task.h"

namespace TexturePackUtils
{

	enum class ProcessingLevel
	{
		Full,
		BasicInfoOnly
	};

	bool process(TexturePack& pack, ProcessingLevel level = ProcessingLevel::Full);

	bool processZIP(TexturePack& pack, ProcessingLevel level = ProcessingLevel::Full);
	bool processFolder(TexturePack& pack, ProcessingLevel level = ProcessingLevel::Full);

	bool processPackTXT(TexturePack& pack, QByteArray&& raw_data);
	bool processPackPNG(const TexturePack& pack, QByteArray&& raw_data);

	/// processes ONLY the pack.png (rest of the pack may be invalid)
	bool processPackPNG(const TexturePack& pack);

	/** Checks whether a file is valid as a texture pack or not. */
	bool validate(QFileInfo file);
} // namespace TexturePackUtils

class LocalTexturePackParseTask : public Task
{
	Q_OBJECT
  public:
	LocalTexturePackParseTask(int token, TexturePack& rp);

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

	TexturePack& m_texture_pack;

	bool m_aborted = false;
};
