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
#include "OverrideUtils.h"

#include <QDirIterator>

#include "FileSystem.h"

namespace Override
{

	void createOverrides(const QString& name, const QString& parent_folder, const QString& override_path)
	{
		QString file_path(FS::PathCombine(parent_folder, name + ".txt"));
		if (QFile::exists(file_path))
			FS::deletePath(file_path);

		FS::ensureFilePathExists(file_path);

		QFile file(file_path);
		if (!file.open(QFile::WriteOnly))
		{
			qWarning() << "Failed to open file '" << file.fileName() << "' for writing!";
			return;
		}

		QDirIterator override_iterator(override_path, QDirIterator::Subdirectories);
		while (override_iterator.hasNext())
		{
			auto override_file_path = override_iterator.next();
			QFileInfo info(override_file_path);
			if (info.isFile())
			{
				// Absolute path with temp directory -> relative path
				override_file_path = override_file_path.split(name).last().remove(0, 1);

				file.write(override_file_path.toUtf8());
				file.write("\n");
			}
		}

		file.close();
	}

	QStringList readOverrides(const QString& name, const QString& parent_folder)
	{
		QString file_path(FS::PathCombine(parent_folder, name + ".txt"));

		QFile file(file_path);
		if (!file.exists())
			return {};

		QStringList previous_overrides;

		if (!file.open(QFile::ReadOnly))
		{
			qWarning() << "Failed to open file '" << file.fileName() << "' for reading!";
			return previous_overrides;
		}

		QString entry;
		do
		{
			entry = file.readLine();
			previous_overrides.append(entry.trimmed());
		}
		while (!entry.isEmpty());

		file.close();

		return previous_overrides;
	}

} // namespace Override
