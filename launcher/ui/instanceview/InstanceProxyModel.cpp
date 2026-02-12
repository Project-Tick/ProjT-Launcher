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
 * ======================================================================== */

#include "InstanceProxyModel.h"

#include <BaseInstance.h>
#include <icons/IconList.hpp>
#include "Application.h"
#include "InstanceView.h"

#include <QDebug>

#include "settings/Setting.h"
#include "settings/SettingsObject.h"

InstanceProxyModel::InstanceProxyModel(QObject* parent) : QSortFilterProxyModel(parent)
{
	m_naturalSort.setNumericMode(true);
	m_naturalSort.setCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);
	updateLocale();

	// Update locale when language-related settings change
	if (auto settings = APPLICATION->settings())
	{
		auto updateIfLanguageSettingChanged = [this](const Setting& setting, const QVariant&)
		{
			if (setting.id() == "Language" || setting.id() == "UseSystemLocale")
				updateLocale();
		};
		auto updateIfLanguageSettingReset = [this](const Setting& setting)
		{
			if (setting.id() == "Language" || setting.id() == "UseSystemLocale")
				updateLocale();
		};

		connect(settings.get(), &SettingsObject::SettingChanged, this, updateIfLanguageSettingChanged);
		connect(settings.get(), &SettingsObject::settingReset, this, updateIfLanguageSettingReset);
	}
}

void InstanceProxyModel::updateLocale()
{
	// Use the current UI locale from Application settings
	auto language = APPLICATION->settings()->get("Language").toString();
	if (!language.isEmpty())
	{
		m_naturalSort.setLocale(QLocale(language));
	}
	else
	{
		m_naturalSort.setLocale(QLocale::system());
	}
	// Trigger re-sort with new locale
	invalidate();
}

QVariant InstanceProxyModel::data(const QModelIndex& index, int role) const
{
	QVariant data = QSortFilterProxyModel::data(index, role);
	if (role == Qt::DecorationRole)
	{
		return QVariant(APPLICATION->icons()->getIcon(data.toString()));
	}
	return data;
}

bool InstanceProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
	const QString leftCategory	= left.data(InstanceViewRoles::GroupRole).toString();
	const QString rightCategory = right.data(InstanceViewRoles::GroupRole).toString();
	if (leftCategory == rightCategory)
	{
		return subSortLessThan(left, right);
	}
	else
	{
		// NOTE: Group visual positioning is handled in InstanceView::updateGeometries() with LocaleString.
		// This comparison ensures logical sorting order matches the visual layout.
		auto result = leftCategory.localeAwareCompare(rightCategory);
		if (result == 0)
		{
			return subSortLessThan(left, right);
		}
		return result < 0;
	}
}

bool InstanceProxyModel::subSortLessThan(const QModelIndex& left, const QModelIndex& right) const
{
	BaseInstance* pdataLeft	 = static_cast<BaseInstance*>(left.internalPointer());
	BaseInstance* pdataRight = static_cast<BaseInstance*>(right.internalPointer());
	QString sortMode		 = APPLICATION->settings()->get("InstSortMode").toString();
	if (sortMode == "LastLaunch")
	{
		return pdataLeft->lastLaunch() > pdataRight->lastLaunch();
	}
	else
	{
		return m_naturalSort.compare(pdataLeft->name(), pdataRight->name()) < 0;
	}
}
