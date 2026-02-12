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
 * Copyright 2015-2021 MultiMC Contributors
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
 * ======================================================================== */

#pragma once
#include <QDateTime>
#include <QFileInfo>
#include <optional>

struct GameType
{
	GameType() = default;
	GameType(std::optional<int> original);

	QString toTranslatedString() const;
	QString toLogString() const;

	enum
	{
		Unknown = -1,
		Survival,
		Creative,
		Adventure,
		Spectator
	} type = Unknown;
	std::optional<int> original;
};

class World
{
  public:
	World(const QFileInfo& file);
	QString folderName() const
	{
		return m_folderName;
	}
	QString name() const
	{
		return m_actualName;
	}
	QString iconFile() const
	{
		return m_iconFile;
	}
	int64_t bytes() const
	{
		return m_size;
	}
	QDateTime lastPlayed() const
	{
		return m_lastPlayed;
	}
	GameType gameType() const
	{
		return m_gameType;
	}
	int64_t seed() const
	{
		return m_randomSeed;
	}
	bool isValid() const
	{
		return m_isValid;
	}
	bool isOnFS() const
	{
		return m_containerFile.isDir();
	}
	QFileInfo container() const
	{
		return m_containerFile;
	}
	// delete all the files of this world
	bool destroy();
	// replace this world with a copy of the other
	bool replace(World& with);
	// change the world's filesystem path (used by world lists for *MAGIC* purposes)
	void repath(const QFileInfo& file);
	// remove the icon file, if any
	bool resetIcon();

	bool rename(const QString& to);
	bool install(const QString& to, const QString& name = QString());

	void setSize(int64_t size);

	// WEAK compare operator - used for replacing worlds
	bool operator==(const World& other) const;

	auto isSymLink() const -> bool
	{
		return m_containerFile.isSymLink();
	}

	/**
	 * @brief Take a instance path, checks if the file pointed to by the resource is a symlink or under a symlink in
	 * that instance
	 *
	 * @param instPath path to an instance directory
	 * @return true
	 * @return false
	 */
	bool isSymLinkUnder(const QString& instPath) const;

	bool isMoreThanOneHardLink() const;

	QString canonicalFilePath() const
	{
		return m_containerFile.canonicalFilePath();
	}

  private:
	void readFromZip(const QFileInfo& file);
	void readFromFS(const QFileInfo& file);
	void loadFromLevelDat(QByteArray data);

  protected:
	QFileInfo m_containerFile;
	QString m_containerOffsetPath;
	QString m_folderName;
	QString m_actualName;
	QString m_iconFile;
	QDateTime m_levelDatTime;
	QDateTime m_lastPlayed;
	int64_t m_size;
	int64_t m_randomSeed = 0;
	GameType m_gameType;
	bool m_isValid = false;
};
