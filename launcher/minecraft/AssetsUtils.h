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

#pragma once

#include <QDir>
#include <QMap>
#include <QString>
#include <optional>
#include "net/NetJob.h"
#include "net/NetRequest.h"

/** Represents a single Minecraft asset object.
 *  Assets are stored by their SHA1 hash in a content-addressable storage.
 */
struct AssetObject
{
	QString hash;
	qint64 size = 0;

	/** Get the relative path within the objects directory (e.g., "ab/abcdef..."). */
	QString getRelPath() const;

	/** Get the full download URL for this asset. */
	QUrl getUrl() const;

	/** Get the local filesystem path where this asset is stored. */
	QString getLocalPath() const;

	/** Create a download action for this asset if it needs to be downloaded. */
	Net::NetRequest::Ptr getDownloadAction() const;

	/** Check if the asset exists locally and has the correct size. */
	bool isValid() const;
};

/** Represents a Minecraft assets index.
 *  The index maps virtual paths to asset objects.
 */
struct AssetsIndex
{
	QString id;
	QMap<QString, AssetObject> objects;
	bool isVirtual		= false;
	bool mapToResources = false;

	/** Create a NetJob to download all missing assets. */
	NetJob::Ptr getDownloadJob() const;

	/** Get the total size of all assets in bytes. */
	qint64 totalSize() const;

	/** Get the number of assets that need to be downloaded. */
	int missingAssetCount() const;

	/** Check if all assets are present locally. */
	bool isComplete() const;
};

/** Asset management utilities.
 *  Provides functions for loading, validating, and reconstructing Minecraft assets.
 */
class AssetsManager
{
  public:
	/** Load an assets index from a JSON file.
	 *  @param id The asset index ID (e.g., "1.19", "legacy")
	 *  @param filePath Path to the index JSON file
	 *  @return The parsed AssetsIndex, or nullopt on failure
	 */
	static std::optional<AssetsIndex> loadIndex(const QString& id, const QString& filePath);

	/** Get the appropriate assets directory for an index.
	 *  Handles virtual assets, legacy resource mapping, and standard paths.
	 *  @param index The assets index
	 *  @param resourcesFolder Path to the resources folder (for legacy versions)
	 *  @return The directory where assets should be accessed
	 */
	static QDir getAssetsDir(const AssetsIndex& index, const QString& resourcesFolder);

	/** Get the assets directory by ID (loads index internally).
	 *  @param assetsId The asset index ID
	 *  @param resourcesFolder Path to the resources folder
	 *  @return The directory where assets should be accessed
	 */
	static QDir getAssetsDir(const QString& assetsId, const QString& resourcesFolder);

	/** Reconstruct virtual/legacy assets from the object store.
	 *  Copies assets from hash-based storage to named paths for older MC versions.
	 *  @param index The assets index to reconstruct
	 *  @param resourcesFolder Path to the resources folder
	 *  @return true if reconstruction succeeded
	 */
	static bool reconstructAssets(const AssetsIndex& index, const QString& resourcesFolder);

	/** Reconstruct assets by ID (loads index internally). */
	static bool reconstructAssets(const QString& assetsId, const QString& resourcesFolder);

	/** Get the standard paths for asset storage. */
	static QDir assetsBaseDir();
	static QDir indexesDir();
	static QDir objectsDir();
	static QDir virtualDir();

	/** Validate the integrity of all assets in an index.
	 *  Checks that all files exist and have correct sizes.
	 *  @param index The assets index to validate
	 *  @return List of missing or invalid asset paths
	 */
	static QStringList validateAssets(const AssetsIndex& index);
};

// Legacy namespace - delegates to AssetsManager
namespace AssetsUtils
{
	bool loadAssetsIndexJson(const QString& id, const QString& file, AssetsIndex& index);
	QDir getAssetsDir(const QString& assetsId, const QString& resourcesFolder);
	bool reconstructAssets(QString assetsId, QString resourcesFolder);
}
