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
#include "InstanceTask.h"

#include "Application.h"
#include "settings/SettingsObject.h"
#include "ui/dialogs/CustomMessageBox.h"

#include <QPushButton>

InstanceNameChange askForChangingInstanceName(QWidget* parent, const QString& old_name, const QString& new_name)
{
	auto dialog = CustomMessageBox::selectable(
		parent,
		QObject::tr("Change instance name"),
		QObject::tr("The instance's name seems to include the old version. Would you like to update it?\n\n"
					"Old name: %1\n"
					"New name: %2")
			.arg(old_name, new_name),
		QMessageBox::Question,
		QMessageBox::No | QMessageBox::Yes);
	auto result = dialog->exec();

	if (result == QMessageBox::Yes)
		return InstanceNameChange::ShouldChange;
	return InstanceNameChange::ShouldKeep;
}

ShouldUpdate askIfShouldUpdate(QWidget* parent, QString original_version_name)
{
	if (APPLICATION->settings()->get("SkipModpackUpdatePrompt").toBool())
		return ShouldUpdate::SkipUpdating;

	auto info = CustomMessageBox::selectable(
		parent,
		QObject::tr("Similar modpack was found!"),
		QObject::tr("One or more of your instances are from this same modpack%1. Do you want to create a "
					"separate instance, or update the existing one?\n\nNOTE: Make sure you made a backup of your "
					"important instance data before "
					"updating, as worlds can be corrupted and some configuration may be lost (due to pack overrides).")
			.arg(original_version_name),
		QMessageBox::Information,
		QMessageBox::Cancel);
	QAbstractButton* update = info->addButton(QObject::tr("Update existing instance"), QMessageBox::AcceptRole);
	QAbstractButton* skip	= info->addButton(QObject::tr("Create new instance"), QMessageBox::ResetRole);

	info->exec();

	if (info->clickedButton() == update)
		return ShouldUpdate::Update;
	if (info->clickedButton() == skip)
		return ShouldUpdate::SkipUpdating;
	return ShouldUpdate::Cancel;
}

QString InstanceName::name() const
{
	if (!m_modified_name.isEmpty())
		return modifiedName();
	if (!m_original_version.isEmpty())
		return QString("%1 %2").arg(m_original_name, m_original_version);

	return m_original_name;
}

QString InstanceName::originalName() const
{
	return m_original_name;
}

QString InstanceName::modifiedName() const
{
	if (!m_modified_name.isEmpty())
		return m_modified_name;
	return m_original_name;
}

QString InstanceName::version() const
{
	return m_original_version;
}

void InstanceName::setName(InstanceName& other)
{
	m_original_name	   = other.m_original_name;
	m_original_version = other.m_original_version;
	m_modified_name	   = other.m_modified_name;
}

InstanceTask::InstanceTask() : Task(), InstanceName()
{}
