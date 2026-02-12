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
 *
 *
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (c) 2022 Jamie Mansfield <jmansfield@cadixdev.org>
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
 *      Copyright 2020-2021 Jamie Mansfield <jmansfield@cadixdev.org>
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

#include "AtlUserInteractionSupportImpl.h"
#include <QMessageBox>

#include "AtlOptionalModDialog.h"
#include "ui/dialogs/VersionSelectDialog.h"

AtlUserInteractionSupportImpl::AtlUserInteractionSupportImpl(QWidget* parent) : m_parent(parent)
{}

std::optional<QList<QString>> AtlUserInteractionSupportImpl::chooseOptionalMods(const ATLauncher::PackVersion& version,
																				QList<ATLauncher::VersionMod> mods)
{
	AtlOptionalModDialog optionalModDialog(m_parent, version, mods);
	auto result = optionalModDialog.exec();
	if (result == QDialog::Rejected)
	{
		return {};
	}
	return optionalModDialog.getResult();
}

QString AtlUserInteractionSupportImpl::chooseVersion(projt::meta::MetaVersionList::Ptr vlist, QString minecraftVersion)
{
	VersionSelectDialog vselect(vlist.get(), "Choose Version", m_parent, false);
	if (minecraftVersion != nullptr)
	{
		vselect.setExactFilter(BaseVersionList::ParentVersionRole, minecraftVersion);
		vselect.setEmptyString(tr("No versions are currently available for Minecraft %1").arg(minecraftVersion));
	}
	else
	{
		vselect.setEmptyString(tr("No versions are currently available"));
	}
	vselect.setEmptyErrorString(tr("Couldn't load or download the version lists!"));

	// select recommended build
	for (int i = 0; i < vlist->allVersions().size(); i++)
	{
		auto version = vlist->allVersions().at(i);
		auto reqs	 = version->dependencies();

		// filter by minecraft version, if the loader depends on a certain version.
		if (minecraftVersion != nullptr)
		{
			auto iter =
				std::find_if(reqs.begin(),
							 reqs.end(),
							 [](const projt::meta::ComponentDependency& req) { return req.uid == "net.minecraft"; });
			if (iter == reqs.end())
				continue;
			if (iter->equalsVersion != minecraftVersion)
				continue;
		}

		// first recommended build we find, we use.
		if (version->isStable())
		{
			vselect.setCurrentVersion(version->descriptor());
			break;
		}
	}

	vselect.exec();
	return vselect.selectedVersion()->descriptor();
}

void AtlUserInteractionSupportImpl::displayMessage(QString message)
{
	QMessageBox::information(m_parent, tr("Installing"), message);
}
