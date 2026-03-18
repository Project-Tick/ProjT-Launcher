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

#include "CapeListModel.h"

#include <QDir>
#include <QFileInfo>
#include <QPainter>

#include "Application.h"
#include "FileSystem.h"
#include "net/Download.h"

CapeListModel::CapeListModel(QObject* parent) : QAbstractListModel(parent)
{}

void CapeListModel::loadFromAccount(MinecraftAccountPtr account, const QString& cacheDir)
{
	beginResetModel();

	m_account  = account;
	m_cacheDir = cacheDir;
	m_capes.clear();
	m_capeImages.clear();
	m_capeIndexMap.clear();

	if (!m_account || !m_account->accountData())
	{
		endResetModel();
		return;
	}

	// Ensure cache directory exists
	QDir().mkpath(m_cacheDir);

	auto& accountData = *m_account->accountData();

	// Add "No Cape" option first
	CapeInfo noCape;
	noCape.id	  = "no_cape";
	noCape.alias  = tr("No Cape");
	noCape.loaded = true;
	m_capes.append(noCape);
	m_capeIndexMap["no_cape"] = 0;

	// Add all available capes
	int index = 1;
	for (const auto& cape : accountData.minecraftProfile.capes)
	{
		CapeInfo info;
		info.id		= cape.id;
		info.alias	= cape.alias;
		info.url	= cape.url;
		info.loaded = false;

		// Check if we have embedded data
		if (!cape.data.isEmpty())
		{
			QImage capeImage;
			if (capeImage.loadFromData(cape.data, "PNG"))
			{
				m_capeImages[cape.id] = capeImage;
				info.loaded			  = true;

				// Also save to cache
				QString cachePath = FS::PathCombine(m_cacheDir, cape.id + ".png");
				capeImage.save(cachePath);
			}
		}

		// Try loading from cache if not embedded
		if (!info.loaded)
		{
			loadCapeFromDisk(cape.id);
			if (m_capeImages.contains(cape.id))
			{
				info.loaded = true;
			}
		}

		m_capes.append(info);
		m_capeIndexMap[cape.id] = index++;
	}

	endResetModel();

	// Download any missing capes
	downloadCapes();
}

void CapeListModel::refresh()
{
	if (m_account)
	{
		loadFromAccount(m_account, m_cacheDir);
	}
}

void CapeListModel::loadCapeFromDisk(const QString& capeId)
{
	QString path = FS::PathCombine(m_cacheDir, capeId + ".png");
	if (QFileInfo::exists(path))
	{
		QImage img;
		if (img.load(path))
		{
			m_capeImages[capeId] = img;
		}
	}
}

void CapeListModel::downloadCapes()
{
	QList<int> toDownload;

	for (int i = 0; i < m_capes.size(); ++i)
	{
		const auto& cape = m_capes[i];
		if (!cape.loaded && !cape.url.isEmpty() && !m_capeImages.contains(cape.id))
		{
			toDownload.append(i);
		}
	}

	if (toDownload.isEmpty())
	{
		emit loadingFinished();
		return;
	}

	m_downloadJob.reset(new NetJob(tr("Download capes"), APPLICATION->network()));

	for (int idx : toDownload)
	{
		const auto& cape  = m_capes[idx];
		QString cachePath = FS::PathCombine(m_cacheDir, cape.id + ".png");
		m_downloadJob->addNetAction(Net::Download::makeFile(cape.url, cachePath));
	}

	connect(m_downloadJob.get(), &NetJob::succeeded, this, &CapeListModel::onDownloadSucceeded);
	connect(m_downloadJob.get(), &NetJob::failed, this, &CapeListModel::onDownloadFailed);

	m_downloadJob->start();
}

void CapeListModel::onDownloadSucceeded()
{
	m_downloadJob.reset();

	// Load all downloaded capes
	for (auto& cape : m_capes)
	{
		if (!cape.loaded && !cape.id.isEmpty() && cape.id != "no_cape")
		{
			loadCapeFromDisk(cape.id);
			if (m_capeImages.contains(cape.id))
			{
				cape.loaded = true;
				int idx		= m_capeIndexMap.value(cape.id, -1);
				if (idx >= 0)
				{
					emit dataChanged(index(idx), index(idx));
					emit capeLoaded(cape.id);
				}
			}
		}
	}

	emit loadingFinished();
}

void CapeListModel::onDownloadFailed(QString reason)
{
	qWarning() << "Cape download failed:" << reason;
	m_downloadJob.reset();
	emit loadingFinished();
}

QImage CapeListModel::getCapeImage(const QString& capeId) const
{
	return m_capeImages.value(capeId, QImage());
}

int CapeListModel::findCapeIndex(const QString& capeId) const
{
	QString id = capeId;
	if (id.isEmpty())
	{
		id = "no_cape";
	}
	return m_capeIndexMap.value(id, -1);
}

QString CapeListModel::capeIdAt(int index) const
{
	if (index < 0 || index >= m_capes.size())
		return QString();
	return m_capes[index].id;
}

int CapeListModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid())
		return 0;
	return m_capes.size();
}

QVariant CapeListModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= m_capes.size())
		return QVariant();

	const auto& cape = m_capes[index.row()];

	switch (role)
	{
		case Qt::DisplayRole: return cape.alias;

		case Qt::DecorationRole:
		{
			QImage img = m_capeImages.value(cape.id, QImage());
			if (!img.isNull())
			{
				return createCapePreview(img, m_elytraMode);
			}
			return QVariant();
		}

		case CapeIdRole: return cape.id;

		case CapeAliasRole: return cape.alias;

		case CapeImageRole: return m_capeImages.value(cape.id, QImage());

		case CapeUrlRole: return cape.url;

		default: return QVariant();
	}
}

QHash<int, QByteArray> CapeListModel::roleNames() const
{
	QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
	roles[CapeIdRole]			 = "capeId";
	roles[CapeAliasRole]		 = "capeAlias";
	roles[CapeImageRole]		 = "capeImage";
	roles[CapeUrlRole]			 = "capeUrl";
	return roles;
}

QPixmap CapeListModel::createCapePreview(const QImage& capeImage, bool elytra) const
{
	if (elytra)
	{
		auto wing		= capeImage.copy(34, 0, 12, 22);
		QImage mirrored = wing.flipped(Qt::Horizontal);

		QImage combined(wing.width() * 2 - 2, wing.height(), capeImage.format());
		combined.fill(Qt::transparent);

		QPainter painter(&combined);
		painter.drawImage(0, 0, wing);
		painter.drawImage(wing.width() - 2, 0, mirrored);
		painter.end();
		return QPixmap::fromImage(combined.scaled(96, 176, Qt::IgnoreAspectRatio, Qt::FastTransformation));
	}
	return QPixmap::fromImage(
		capeImage.copy(1, 1, 10, 16).scaled(80, 128, Qt::IgnoreAspectRatio, Qt::FastTransformation));
}
