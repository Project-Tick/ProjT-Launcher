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

#include <QFile>
#include <QSet>
#include <QString>

namespace LegacyFTB
{

	class PrivatePackManager
	{
	  public:
		~PrivatePackManager()
		{
			save();
		}
		void load();
		void save() const;
		bool empty() const
		{
			return currentPacks.empty();
		}
		const QSet<QString>& getCurrentPackCodes() const
		{
			return currentPacks;
		}
		void add(const QString& code)
		{
			currentPacks.insert(code);
			dirty = true;
		}
		void remove(const QString& code)
		{
			currentPacks.remove(code);
			dirty = true;
		}

	  private:
		QSet<QString> currentPacks;
		QString m_filename = "private_packs.txt";
		mutable bool dirty = false;
	};

} // namespace LegacyFTB
