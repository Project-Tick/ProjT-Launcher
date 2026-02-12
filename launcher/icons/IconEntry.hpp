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

#include <QIcon>
#include <QString>
#include <functional>

namespace projt::icons
{
	enum IconType : unsigned
	{
		Builtin,
		Transient,
		FileBased,
		ICONS_TOTAL,
		ToBeDeleted
	};

	struct IconImage
	{
		QIcon icon;
		QString key;
		QString filename;
		bool present() const
		{
			return !icon.isNull() || !key.isEmpty();
		}
	};

	struct IconEntry
	{
		QString m_key;
		QString m_name;
		IconImage m_images[ICONS_TOTAL];
		IconType m_current_type = ToBeDeleted;

		IconType type() const;
		QString name() const;
		bool has(IconType _type) const;
		QIcon icon() const;
		void remove(IconType rm_type);
		void replace(IconType new_type, QIcon icon, QString path = QString());
		void replace(IconType new_type, const QString& key);
		bool isBuiltIn() const;
		QString getFilePath() const;

		using ThemeIconProvider = std::function<QIcon(const QString&)>;
		static void setThemeIconProvider(ThemeIconProvider provider);

	  private:
		static ThemeIconProvider s_themeProvider;
	};
} // namespace projt::icons
