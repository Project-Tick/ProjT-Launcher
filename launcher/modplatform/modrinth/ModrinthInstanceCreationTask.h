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

#include <optional>

#include <QByteArray>
#include <QCryptographicHash>
#include <QHash>
#include <QQueue>
#include <QString>
#include <QUrl>
#include <QVector>

#include "BaseInstance.h"
#include "InstanceCreationTask.h"
#include "net/Download.h"

class NetJob;

class ModrinthCreationTask final : public InstanceCreationTask
{
	Q_OBJECT
	struct File
	{
		QString path;

		QCryptographicHash::Algorithm hashAlgorithm;
		QByteArray hash;
		QQueue<QUrl> downloads;
		bool required = true;
	};

  public:
	ModrinthCreationTask(QString staging_path,
						 SettingsObjectPtr global_settings,
						 QWidget* parent,
						 QString id,
						 QString version_id			  = {},
						 QString original_instance_id = {})
		: InstanceCreationTask(),
		  m_parent(parent),
		  m_managed_id(std::move(id)),
		  m_managed_version_id(std::move(version_id))
	{
		setStagingPath(staging_path);
		setParentSettings(global_settings);

		m_original_instance_id = std::move(original_instance_id);
	}

	bool abort() override;

	bool updateInstance() override;
	std::unique_ptr<MinecraftInstance> createInstance() override;

  private:
	bool parseManifest(const QString&,
					   std::vector<File>&,
					   bool set_internal_data	 = true,
					   bool show_optional_dialog = true);
	void attachRetryHandler(Net::Download::Ptr dl, shared_qobject_ptr<NetJob> downloadMods);

  private:
	QWidget* m_parent = nullptr;

	QString m_minecraft_version, m_fabric_version, m_quilt_version, m_forge_version, m_neoForge_version;
	QString m_managed_id, m_managed_version_id, m_managed_name;

	std::vector<File> m_files;
	Task::Ptr m_task;

	std::optional<InstancePtr> m_instance;

	QString m_root_path = "minecraft";

	// Alternative URLs tracking for download retry mechanism
	struct FileDownloadInfo
	{
		QString filePath;
		QQueue<QString> remainingUrls;
		QByteArray hash;
		QCryptographicHash::Algorithm hashAlgorithm;
	};
	QHash<Net::Download*, FileDownloadInfo> m_alternativeUrls;
};
