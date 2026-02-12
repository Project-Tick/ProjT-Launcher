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

#include <QDebug>
#include <QObject>

#include "minecraft/mod/Mod.hpp"
#include "minecraft/mod/ModDetails.hpp"

#include "tasks/Task.h"

namespace ModUtils
{

	ModDetails ReadFabricModInfo(QByteArray contents);
	ModDetails ReadQuiltModInfo(QByteArray contents);
	ModDetails ReadForgeInfo(QByteArray contents);
	ModDetails ReadLiteModInfo(QByteArray contents);

	enum class ProcessingLevel
	{
		Full,
		BasicInfoOnly
	};

	bool process(Mod& mod, ProcessingLevel level = ProcessingLevel::Full);

	bool processZIP(Mod& mod, ProcessingLevel level = ProcessingLevel::Full);
	bool processFolder(Mod& mod, ProcessingLevel level = ProcessingLevel::Full);
	bool processLitemod(Mod& mod, ProcessingLevel level = ProcessingLevel::Full);

	/** Checks whether a file is valid as a mod or not. */
	bool validate(QFileInfo file);

	bool processIconPNG(const Mod& mod, QByteArray&& raw_data, QPixmap* pixmap);
	bool loadIconFile(const Mod& mod, QPixmap* pixmap);
} // namespace ModUtils

class LocalModParseTask : public Task
{
	Q_OBJECT
  public:
	struct Result
	{
		ModDetails details;
	};
	using ResultPtr = std::shared_ptr<Result>;
	ResultPtr result() const
	{
		return m_result;
	}

	bool canAbort() const override
	{
		return true;
	}
	bool abort() override;

	LocalModParseTask(int token, ResourceType type, const QFileInfo& modFile);
	void executeTask() override;

	int token() const
	{
		return m_token;
	}

  private:
	int m_token;
	ResourceType m_type;
	QFileInfo m_modFile;
	ResultPtr m_result;

	std::atomic<bool> m_aborted = false;
};
