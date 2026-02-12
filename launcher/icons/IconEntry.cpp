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

#include "IconEntry.hpp"

namespace projt::icons
{
	IconEntry::ThemeIconProvider IconEntry::s_themeProvider = [](const QString& key) { return QIcon::fromTheme(key); };

	void IconEntry::setThemeIconProvider(ThemeIconProvider provider)
	{
		s_themeProvider = provider;
	}

	IconType operator--(IconType& t, int)
	{
		IconType temp = t;
		switch (t)
		{
			case IconType::Builtin: t = IconType::ToBeDeleted; break;
			case IconType::Transient: t = IconType::Builtin; break;
			case IconType::FileBased: t = IconType::Transient; break;
			default: break;
		}
		return temp;
	}

	IconType IconEntry::type() const
	{
		return m_current_type;
	}

	QString IconEntry::name() const
	{
		if (!m_name.isEmpty())
		{
			return m_name;
		}
		return m_key;
	}

	bool IconEntry::has(IconType _type) const
	{
		return m_images[_type].present();
	}

	QIcon IconEntry::icon() const
	{
		if (m_current_type == IconType::ToBeDeleted)
		{
			return QIcon();
		}
		auto& icon = m_images[m_current_type].icon;
		if (!icon.isNull())
		{
			return icon;
		}

		if (s_themeProvider)
		{
			return s_themeProvider(m_images[m_current_type].key);
		}
		return QIcon::fromTheme(m_images[m_current_type].key);
	}

	void IconEntry::remove(IconType rm_type)
	{
		m_images[rm_type].filename = QString();
		m_images[rm_type].icon	   = QIcon();
		for (auto iter = rm_type; iter != IconType::ToBeDeleted; iter--)
		{
			if (m_images[iter].present())
			{
				m_current_type = iter;
				return;
			}
		}
		m_current_type = IconType::ToBeDeleted;
	}

	void IconEntry::replace(IconType new_type, QIcon icon, QString path)
	{
		if (new_type > m_current_type || m_current_type == IconType::ToBeDeleted)
		{
			m_current_type = new_type;
		}
		m_images[new_type].icon		= icon;
		m_images[new_type].filename = path;
		m_images[new_type].key		= QString();
	}

	void IconEntry::replace(IconType new_type, const QString& key)
	{
		if (new_type > m_current_type || m_current_type == IconType::ToBeDeleted)
		{
			m_current_type = new_type;
		}
		m_images[new_type].icon		= QIcon();
		m_images[new_type].filename = QString();
		m_images[new_type].key		= key;
	}

	QString IconEntry::getFilePath() const
	{
		if (m_current_type == IconType::ToBeDeleted)
		{
			return QString();
		}
		return m_images[m_current_type].filename;
	}

	bool IconEntry::isBuiltIn() const
	{
		return m_current_type == IconType::Builtin;
	}
} // namespace projt::icons
