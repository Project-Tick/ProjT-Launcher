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
 *  Copyright (c) 2022 flowln <flowlnlnln@gmail.com>
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
 *      Copyright 2013-2021 MultiMC Contributors
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

#pragma once

#include <memory>

#include <QString>
#include <QStringList>
#include <QUrl>

#include "minecraft/mod/MetadataHandler.hpp"

struct ModLicense
{
	QString name		= {};
	QString id			= {};
	QString url			= {};
	QString description = {};

	ModLicense()
	{}

	/** Parse a license string which may contain:
	 *  - An SPDX identifier (e.g., "MIT", "GPL-3.0-only", "Apache-2.0")
	 *  - A URL in parentheses or standalone
	 *  - A plain text license name
	 *
	 *  Supports SPDX license expressions: https://spdx.org/licenses/
	 */
	ModLicense(const QString license)
	{
		if (license.isEmpty())
		{
			return;
		}

		QString remaining = license.trimmed();

		// Common SPDX identifiers for quick matching
		static const QStringList spdxIdentifiers = { "MIT",
													 "GPL-2.0",
													 "GPL-2.0-only",
													 "GPL-2.0-or-later",
													 "GPL-3.0",
													 "GPL-3.0-only",
													 "GPL-3.0-or-later",
													 "LGPL-2.0",
													 "LGPL-2.1",
													 "LGPL-3.0",
													 "Apache-2.0",
													 "BSD-2-Clause",
													 "BSD-3-Clause",
													 "MPL-2.0",
													 "ISC",
													 "Unlicense",
													 "WTFPL",
													 "Zlib",
													 "CC0-1.0",
													 "CC-BY-4.0",
													 "CC-BY-SA-4.0",
													 "AGPL-3.0",
													 "AGPL-3.0-only",
													 "AGPL-3.0-or-later",
													 "EPL-2.0",
													 "OSL-3.0",
													 "Artistic-2.0",
													 "BSL-1.0",
													 "All Rights Reserved",
													 "ARR",
													 "Custom",
													 "Proprietary" };

		// SPDX to URL mapping for common licenses
		static const QHash<QString, QString> spdxUrls = {
			{ "MIT", "https://opensource.org/licenses/MIT" },
			{ "GPL-2.0", "https://www.gnu.org/licenses/old-licenses/gpl-2.0.html" },
			{ "GPL-2.0-only", "https://www.gnu.org/licenses/old-licenses/gpl-2.0.html" },
			{ "GPL-2.0-or-later", "https://www.gnu.org/licenses/old-licenses/gpl-2.0.html" },
			{ "GPL-3.0", "https://www.gnu.org/licenses/gpl-3.0.html" },
			{ "GPL-3.0-only", "https://www.gnu.org/licenses/gpl-3.0.html" },
			{ "GPL-3.0-or-later", "https://www.gnu.org/licenses/gpl-3.0.html" },
			{ "LGPL-2.1", "https://www.gnu.org/licenses/lgpl-2.1.html" },
			{ "LGPL-3.0", "https://www.gnu.org/licenses/lgpl-3.0.html" },
			{ "Apache-2.0", "https://www.apache.org/licenses/LICENSE-2.0" },
			{ "BSD-2-Clause", "https://opensource.org/licenses/BSD-2-Clause" },
			{ "BSD-3-Clause", "https://opensource.org/licenses/BSD-3-Clause" },
			{ "MPL-2.0", "https://www.mozilla.org/en-US/MPL/2.0/" },
			{ "ISC", "https://opensource.org/licenses/ISC" },
			{ "Unlicense", "https://unlicense.org/" },
			{ "CC0-1.0", "https://creativecommons.org/publicdomain/zero/1.0/" },
			{ "CC-BY-4.0", "https://creativecommons.org/licenses/by/4.0/" },
			{ "CC-BY-SA-4.0", "https://creativecommons.org/licenses/by-sa/4.0/" },
			{ "AGPL-3.0", "https://www.gnu.org/licenses/agpl-3.0.html" },
			{ "AGPL-3.0-only", "https://www.gnu.org/licenses/agpl-3.0.html" },
		};

		// Extract URL from parentheses or standalone
		auto parts = remaining.split(' ');
		QStringList urlParts;

		for (const auto& part : parts)
		{
			QString urlCandidate = part;
			if (part.startsWith("(") && part.endsWith(")"))
			{
				urlCandidate = part.mid(1, part.size() - 2);
			}

			QUrl parsedUrl(urlCandidate);
			if (parsedUrl.isValid() && !parsedUrl.scheme().isEmpty() && !parsedUrl.host().isEmpty())
			{
				this->url = parsedUrl.toString();
				urlParts.append(part);
			}
		}

		// Remove URL parts from remaining
		for (const auto& urlPart : urlParts)
		{
			parts.removeOne(urlPart);
		}

		QString licensePart = parts.join(' ').trimmed();

		// Check if it's a known SPDX identifier
		for (const QString& spdx : spdxIdentifiers)
		{
			if (licensePart.compare(spdx, Qt::CaseInsensitive) == 0)
			{
				this->id		  = spdx;
				this->name		  = spdx;
				this->description = spdx;

				// Set URL from SPDX mapping if not already set
				if (this->url.isEmpty() && spdxUrls.contains(spdx))
				{
					this->url = spdxUrls[spdx];
				}
				return;
			}
		}

		// Not a known SPDX - treat as custom license
		this->name		  = licensePart;
		this->description = licensePart;

		if (parts.size() == 1)
		{
			this->id = parts.first();
		}
	}

	ModLicense(const QString& name_, const QString& id_, const QString& url_, const QString& description_)
		: name(name_),
		  id(id_),
		  url(url_),
		  description(description_)
	{}

	ModLicense(const ModLicense& other) : name(other.name), id(other.id), url(other.url), description(other.description)
	{}

	ModLicense& operator=(const ModLicense& other)
	{
		this->name		  = other.name;
		this->id		  = other.id;
		this->url		  = other.url;
		this->description = other.description;

		return *this;
	}

	ModLicense& operator=(const ModLicense&& other)
	{
		this->name		  = other.name;
		this->id		  = other.id;
		this->url		  = other.url;
		this->description = other.description;

		return *this;
	}

	bool isEmpty()
	{
		return this->name.isEmpty() && this->id.isEmpty() && this->url.isEmpty() && this->description.isEmpty();
	}
};

struct ModDetails
{
	/* Mod ID as defined in the ModLoader-specific metadata */
	QString mod_id = {};

	/* Human-readable name */
	QString name = {};

	/* Human-readable mod version */
	QString version = {};

	/* Human-readable minecraft version */
	QString mcversion = {};

	/* URL for mod's home page */
	QString homeurl = {};

	/* Human-readable description */
	QString description = {};

	/* List of the author's names */
	QStringList authors = {};

	/* Issue Tracker URL */
	QString issue_tracker = {};

	/* License */
	QList<ModLicense> licenses = {};

	/* Path of mod logo */
	QString icon_file = {};

	ModDetails() = default;

	/** Metadata should be handled manually to properly set the mod status. */
	ModDetails(const ModDetails& other)
		: mod_id(other.mod_id),
		  name(other.name),
		  version(other.version),
		  mcversion(other.mcversion),
		  homeurl(other.homeurl),
		  description(other.description),
		  authors(other.authors),
		  issue_tracker(other.issue_tracker),
		  licenses(other.licenses),
		  icon_file(other.icon_file)
	{}

	ModDetails& operator=(const ModDetails& other) = default;

	ModDetails& operator=(ModDetails&& other) = default;
};
