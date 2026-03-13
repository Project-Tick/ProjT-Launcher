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

#include <QAbstractListModel>
#include <QImage>
#include <QPixmap>
#include <QDir>

#include "minecraft/auth/MinecraftAccount.hpp"
#include "net/NetJob.h"

/**
 * @brief Model for managing Minecraft capes with on-demand loading
 *
 * This model provides a list of available capes for a Minecraft account,
 * downloading cape images on demand and caching them locally.
 */
class CapeListModel : public QAbstractListModel
{
	Q_OBJECT

  public:
	enum Roles
	{
		CapeIdRole = Qt::UserRole,
		CapeAliasRole,
		CapeImageRole,
		CapeUrlRole
	};

	explicit CapeListModel(QObject* parent = nullptr);
	virtual ~CapeListModel() = default;

	/**
	 * @brief Initialize the model with account data
	 * @param account The Minecraft account to load capes from
	 * @param cacheDir Directory to cache cape images
	 */
	void loadFromAccount(MinecraftAccountPtr account, const QString& cacheDir);

	/**
	 * @brief Refresh cape data from the account
	 */
	void refresh();

	/**
	 * @brief Get cape image by ID
	 * @param capeId The cape identifier
	 * @return Cape image or null image if not loaded
	 */
	QImage getCapeImage(const QString& capeId) const;

	/**
	 * @brief Get all loaded cape images
	 * @return Hash of cape ID to image
	 */
	QHash<QString, QImage> allCapes() const
	{
		return m_capeImages;
	}

	/**
	 * @brief Find index of cape by ID
	 * @param capeId The cape identifier
	 * @return Index in model or -1 if not found
	 */
	int findCapeIndex(const QString& capeId) const;

	/**
	 * @brief Get cape ID at index
	 * @param index Model index
	 * @return Cape ID or empty string
	 */
	QString capeIdAt(int index) const;

	// QAbstractListModel interface
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	QHash<int, QByteArray> roleNames() const override;

  signals:
	/**
	 * @brief Emitted when a cape image has been loaded
	 * @param capeId The cape that was loaded
	 */
	void capeLoaded(const QString& capeId);

	/**
	 * @brief Emitted when all capes have finished loading
	 */
	void loadingFinished();

  private slots:
	void onDownloadSucceeded();
	void onDownloadFailed(QString reason);

  private:
	struct CapeInfo
	{
		QString id;
		QString alias;
		QString url;
		bool loaded = false;
	};

	void downloadCapes();
	void loadCapeFromDisk(const QString& capeId);
	QPixmap createCapePreview(const QImage& capeImage, bool elytra = false) const;

	MinecraftAccountPtr m_account;
	QString m_cacheDir;
	QList<CapeInfo> m_capes;
	QHash<QString, QImage> m_capeImages;
	QHash<QString, int> m_capeIndexMap;
	NetJob::Ptr m_downloadJob;
	bool m_elytraMode = false;
};
