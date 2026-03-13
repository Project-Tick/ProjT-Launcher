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
 *
 *
 *
 *
 *
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (C) 2022 Rachel Powers <508861+Ryex@users.noreply.github.com>
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
 * ======================================================================== */

#pragma once

#include "Resource.hpp"

#include <QHash>
#include <QLocale>
#include <QMutex>
#include <QPixmapCache>

class Version;

class DataPack : public Resource
{
	Q_OBJECT
  public:
	DataPack(QObject* parent = nullptr) : Resource(parent)
	{}
	DataPack(QFileInfo file_info) : Resource(file_info)
	{}

	/** Gets the numerical ID of the pack format. */
	int packFormat() const
	{
		return m_pack_format;
	}
	/** Gets, respectively, the lower and upper versions supported by the set pack format. */
	virtual std::pair<Version, Version> compatibleVersions() const;

	/** Gets the description of the data pack.
	 *  If a localized description exists for the current locale, returns that.
	 *  Otherwise returns the default description.
	 */
	QString description() const
	{
		QString lang = QLocale::system().name().section('_', 0, 0);
		if (m_localized_descriptions.contains(lang))
		{
			return m_localized_descriptions.value(lang);
		}
		return m_description;
	}

	/** Gets the raw (non-localized) description. */
	QString rawDescription() const
	{
		return m_description;
	}

	/** Gets description for a specific language code. */
	QString localizedDescription(const QString& langCode) const
	{
		return m_localized_descriptions.value(langCode, m_description);
	}

	/** Gets all available localized descriptions. */
	QHash<QString, QString> allLocalizedDescriptions() const
	{
		return m_localized_descriptions;
	}

	/** Gets the image of the data pack, converted to a QPixmap for drawing, and scaled to size. */
	QPixmap image(QSize size, Qt::AspectRatioMode mode = Qt::AspectRatioMode::IgnoreAspectRatio) const;

	/** Thread-safe. */
	void setPackFormat(int new_format_id);

	/** Thread-safe. */
	void setDescription(QString new_description);

	/** Thread-safe. Sets a localized description for a specific language. */
	void setLocalizedDescription(const QString& langCode, const QString& description);

	/** Thread-safe. Sets all localized descriptions at once. */
	void setLocalizedDescriptions(const QHash<QString, QString>& descriptions);

	/** Thread-safe. */
	void setImage(QImage new_image) const;

	bool valid() const override;

	[[nodiscard]] int compare(Resource const& other, SortType type) const override;
	[[nodiscard]] bool applyFilter(QRegularExpression filter) const override;

  protected:
	mutable QMutex m_data_lock;

	/* The 'version' of a data pack, as defined in the pack.mcmeta file.
	 * See https://minecraft.wiki/w/Data_pack#pack.mcmeta
	 */
	int m_pack_format = 0;

	/** The data pack's description, as defined in the pack.mcmeta file.
	 */
	QString m_description;

	/** Localized descriptions keyed by language code (e.g., "en", "de", "ja").
	 *  Parsed from pack.mcmeta language files if available.
	 */
	QHash<QString, QString> m_localized_descriptions;

	/** The data pack's image file cache key, for access in the QPixmapCache global instance.
	 *
	 *  The 'was_ever_used' state simply identifies whether the key was never inserted on the cache (true),
	 *  so as to tell whether a cache entry is inexistent or if it was just evicted from the cache.
	 */
	struct
	{
		QPixmapCache::Key key;
		bool was_ever_used = false;
	} mutable m_pack_image_cache_key;
};
