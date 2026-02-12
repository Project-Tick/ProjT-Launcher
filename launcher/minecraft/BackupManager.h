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
 */

#pragma once

#include <QDateTime>
#include <QFutureWatcher>
#include <QObject>
#include <QString>
#include <QStringList>
#include "BaseInstance.h"

struct BackupOptions
{
	bool includeSaves		  = true;
	bool includeConfig		  = true;
	bool includeMods		  = false;
	bool includeResourcePacks = false;
	bool includeShaderPacks	  = false;
	bool includeScreenshots	  = false;
	bool includeOptions		  = true;
	QStringList customPaths; // Relative paths to include (e.g., "logs", "crash-reports")

	qint64 estimateSize() const;
};

class InstanceBackup
{
  public:
	InstanceBackup() = default;
	InstanceBackup(const QString& path);

	QString name() const
	{
		return m_name;
	}
	QString backupPath() const
	{
		return m_backupPath;
	}
	QDateTime createdAt() const
	{
		return m_createdAt;
	}
	qint64 size() const
	{
		return m_size;
	}
	QString description() const
	{
		return m_description;
	}
	QStringList includedPaths() const
	{
		return m_includedPaths;
	}

	void setName(const QString& name)
	{
		m_name = name;
	}
	void setDescription(const QString& desc)
	{
		m_description = desc;
	}

	bool isValid() const;
	QString displaySize() const;

  private:
	QString m_name;
	QString m_backupPath;
	QString m_description;
	QDateTime m_createdAt;
	qint64 m_size = 0;
	QStringList m_includedPaths;

	friend class BackupManager;
};

class BackupManager : public QObject
{
	Q_OBJECT

  public:
	explicit BackupManager(QObject* parent = nullptr);

	// Create a new backup (async)
	void createBackupAsync(InstancePtr instance, const QString& backupName, const BackupOptions& options);

	// Create a new backup (blocking)
	bool createBackup(InstancePtr instance, const QString& backupName, const BackupOptions& options);

	// Restore from backup (async)
	void restoreBackupAsync(InstancePtr instance, const InstanceBackup& backup, bool createBackupBeforeRestore = true);

	// Restore from backup (blocking)
	bool restoreBackup(InstancePtr instance, const InstanceBackup& backup, bool createBackupBeforeRestore = true);

	// List all backups for an instance
	QList<InstanceBackup> listBackups(InstancePtr instance) const;

	// Delete old backups (keep only maxCount newest)
	int deleteOldBackups(InstancePtr instance, int maxCount);

	// Delete specific backup
	bool deleteBackup(const InstanceBackup& backup);

	// Get backup directory for instance
	static QString getBackupDirectory(InstancePtr instance);

	// Auto backup before launch
	bool autoBackupBeforeLaunch(InstancePtr instance);

  signals:
	void backupCreated(const QString& instanceId, const QString& backupName);
	void backupRestored(const QString& instanceId, const QString& backupName);
	void backupDeleted(const QString& instanceId, const QString& backupName);
	void backupProgress(int current, int total, const QString& currentFile);
	void backupStarted(const QString& instanceId, const QString& backupName);
	void backupFailed(const QString& instanceId, const QString& error);
	void restoreStarted(const QString& instanceId, const QString& backupName);
	void restoreFailed(const QString& instanceId, const QString& error);

  private:
	bool compressBackup(const QString& sourcePath, const QString& backupPath, const BackupOptions& options);
	bool extractBackup(const QString& backupPath, const QString& targetPath);
	QList<InstanceBackup> scanBackupDirectory(const QString& backupDir) const;
};