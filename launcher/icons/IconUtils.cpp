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

#include "IconUtils.hpp"

#include <QDirIterator>
#include "FileSystem.h"

namespace
{
	static const QStringList validIconExtensions = { { "svg", "png", "ico", "gif", "jpg", "jpeg", "webp" } };
}

namespace projt::icons
{

	QString findBestIconIn(const QString& folder, const QString& iconKey)
	{
		QString best_filename;

		QDirIterator it(folder, QDir::NoDotAndDotDot | QDir::Files, QDirIterator::NoIteratorFlags);
		while (it.hasNext())
		{
			it.next();
			auto fileInfo = it.fileInfo();
			if ((fileInfo.completeBaseName() == iconKey || fileInfo.fileName() == iconKey)
				&& isIconSuffix(fileInfo.suffix()))
				return fileInfo.absoluteFilePath();
		}
		return {};
	}

	QString getIconFilter()
	{
		return "(*." + validIconExtensions.join(" *.") + ")";
	}

	bool isIconSuffix(QString suffix)
	{
		return validIconExtensions.contains(suffix);
	}

} // namespace projt::icons
