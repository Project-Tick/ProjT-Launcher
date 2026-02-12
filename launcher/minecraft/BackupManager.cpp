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

#include "BackupManager.h"
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtConcurrent>
#include "FileSystem.h"
#include "MMCZip.h"
#include "MinecraftInstance.h"

// BackupOptions implementation
qint64 BackupOptions::estimateSize() const
{
	// Rough estimation based on typical Minecraft instance sizes
	// Note: This is a rough estimate for UI display. Actual size may vary.
	qint64 size = 0;
	if (includeSaves)
		size += 500LL * 1024 * 1024; // 500MB average for saves
	if (includeConfig)
		size += 50LL * 1024 * 1024; // 50MB for config
	if (includeMods)
		size += 1000LL * 1024 * 1024; // 1GB for mods
	if (includeResourcePacks)
		size += 200LL * 1024 * 1024; // 200MB
	if (includeShaderPacks)
		size += 100LL * 1024 * 1024; // 100MB
	if (includeScreenshots)
		size += 100LL * 1024 * 1024; // 100MB
	if (includeOptions)
		size += 1LL * 1024 * 1024; // 1MB
	// Add custom paths roughly - assume 50MB per path as placeholder
	size += customPaths.size() * 50LL * 1024 * 1024; // 50MB per custom path
	return size;
}

// InstanceBackup implementation
InstanceBackup::InstanceBackup(const QString& path) : m_backupPath(path)
{
	QFileInfo info(path);
	if (!info.exists())
	{
		return;
	}

	m_size		= info.size();
	m_createdAt = info.birthTime();
	m_name		= info.completeBaseName();

	// Try to load metadata
	QString metaPath = path + ".json";
	if (QFile::exists(metaPath))
	{
		QFile metaFile(metaPath);
		if (metaFile.open(QIODevice::ReadOnly))
		{
			QJsonDocument doc = QJsonDocument::fromJson(metaFile.readAll());
			QJsonObject obj	  = doc.object();

			if (obj.contains("name"))
				m_name = obj["name"].toString();
			if (obj.contains("description"))
				m_description = obj["description"].toString();
			if (obj.contains("created"))
				m_createdAt = QDateTime::fromString(obj["created"].toString(), Qt::ISODate);
			if (obj.contains("includedPaths"))
				m_includedPaths = obj["includedPaths"].toVariant().toStringList();
		}
	}
}

bool InstanceBackup::isValid() const
{
	return QFile::exists(m_backupPath) && m_size > 0;
}

QString InstanceBackup::displaySize() const
{
	double size		  = m_size;
	QStringList units = { "B", "KB", "MB", "GB" };
	int unitIndex	  = 0;

	while (size >= 1024.0 && unitIndex < units.size() - 1)
	{
		size /= 1024.0;
		unitIndex++;
	}

	return QString::number(size, 'f', 2) + " " + units[unitIndex];
}

// BackupManager implementation
BackupManager::BackupManager(QObject* parent) : QObject(parent)
{}

QString BackupManager::getBackupDirectory(InstancePtr instance)
{
	if (!instance)
	{
		return QString();
	}

	QString backupDir = FS::PathCombine(instance->instanceRoot(), "backups");
	FS::ensureFolderPathExists(backupDir);
	return backupDir;
}

bool BackupManager::createBackup(InstancePtr instance, const QString& backupName, const BackupOptions& options)
{
	if (!instance)
	{
		qWarning() << "BackupManager: instance is null";
		return false;
	}

	QString backupDir = getBackupDirectory(instance);
	qDebug() << "BackupManager: backup directory:" << backupDir;

	QString timestamp  = QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss");
	QString safeName   = backupName.isEmpty() ? timestamp : backupName + "_" + timestamp;
	QString backupPath = FS::PathCombine(backupDir, safeName + ".zip");

	qDebug() << "BackupManager: creating backup at:" << backupPath;

	// Create metadata
	QJsonObject metadata;
	metadata["name"]		 = backupName.isEmpty() ? timestamp : backupName;
	metadata["created"]		 = QDateTime::currentDateTime().toString(Qt::ISODate);
	metadata["instanceName"] = instance->name();

	QStringList includedPaths;
	if (options.includeSaves)
		includedPaths << "saves";
	if (options.includeConfig)
		includedPaths << "config";
	if (options.includeMods)
		includedPaths << "mods";
	if (options.includeResourcePacks)
		includedPaths << "resourcepacks";
	if (options.includeShaderPacks)
		includedPaths << "shaderpacks";
	if (options.includeScreenshots)
		includedPaths << "screenshots";
	if (options.includeOptions)
		includedPaths << "options.txt"
					  << "optionsof.txt";

	// Add custom paths to included paths list
	includedPaths += options.customPaths;

	metadata["includedPaths"] = QJsonArray::fromStringList(includedPaths);

	// Save metadata
	QFile metaFile(backupPath + ".json");
	if (metaFile.open(QIODevice::WriteOnly))
	{
		metaFile.write(QJsonDocument(metadata).toJson());
		metaFile.close();
	}
	else
	{
		qWarning() << "BackupManager: failed to write metadata file";
	}

	// Get the game root (where .minecraft files are)
	auto minecraftInstance = std::dynamic_pointer_cast<MinecraftInstance>(instance);
	QString gameRoot	   = minecraftInstance ? minecraftInstance->gameRoot() : instance->instanceRoot();

	// Compress backup
	qDebug() << "BackupManager: starting compression...";
	qDebug() << "BackupManager: game root is:" << gameRoot;
	if (!compressBackup(gameRoot, backupPath, options))
	{
		qWarning() << "BackupManager: compression failed";
		QFile::remove(backupPath);
		QFile::remove(backupPath + ".json");
		return false;
	}

	qDebug() << "BackupManager: backup created successfully";
	emit backupCreated(instance->id(), backupName);
	return true;
}

void BackupManager::createBackupAsync(InstancePtr instance, const QString& backupName, const BackupOptions& options)
{
	if (!instance)
	{
		emit backupFailed(QString(), "Instance is null");
		return;
	}

	QString instanceId = instance->id();
	emit backupStarted(instanceId, backupName);

	// Run backup in background thread
	auto future = QtConcurrent::run([this, instance, backupName, options]()
									{ return createBackup(instance, backupName, options); });

	// Watch for completion
	auto watcher = new QFutureWatcher<bool>(this);
	connect(watcher,
			&QFutureWatcher<bool>::finished,
			this,
			[this, watcher, instanceId, backupName]()
			{
				bool success = watcher->result();
				if (!success)
				{
					emit backupFailed(instanceId, "Backup creation failed");
				}
				watcher->deleteLater();
			});
	watcher->setFuture(future);
}

void BackupManager::restoreBackupAsync(InstancePtr instance,
									   const InstanceBackup& backup,
									   bool createBackupBeforeRestore)
{
	if (!instance)
	{
		emit restoreFailed(QString(), "Instance is null");
		return;
	}

	QString instanceId = instance->id();
	QString backupName = backup.name();
	emit restoreStarted(instanceId, backupName);

	// Run restore in background thread
	auto future = QtConcurrent::run([this, instance, backup, createBackupBeforeRestore]()
									{ return restoreBackup(instance, backup, createBackupBeforeRestore); });

	// Watch for completion
	auto watcher = new QFutureWatcher<bool>(this);
	connect(watcher,
			&QFutureWatcher<bool>::finished,
			this,
			[this, watcher, instanceId, backupName]()
			{
				bool success = watcher->result();
				if (!success)
				{
					emit restoreFailed(instanceId, "Backup restoration failed");
				}
				watcher->deleteLater();
			});
	watcher->setFuture(future);
}

bool BackupManager::compressBackup(const QString& sourcePath, const QString& backupPath, const BackupOptions& options)
{
	QFileInfoList files;

	qDebug() << "BackupManager: compressing from" << sourcePath << "to" << backupPath;
	qDebug() << "BackupManager: backup options - saves:" << options.includeSaves << "config:" << options.includeConfig
			 << "mods:" << options.includeMods << "resourcepacks:" << options.includeResourcePacks
			 << "shaderpacks:" << options.includeShaderPacks << "screenshots:" << options.includeScreenshots
			 << "options:" << options.includeOptions << "customPaths:" << options.customPaths;

	// Helper lambda to recursively collect files
	auto collectFilesRecursive = [&files](const QString& dirPath)
	{
		qDebug() << "BackupManager: scanning directory" << dirPath;
		int count = 0;
		QDirIterator it(dirPath, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
		while (it.hasNext())
		{
			QFileInfo info(it.next());
			files.append(info);
			count++;
		}
		qDebug() << "BackupManager: found" << count << "files in" << dirPath;
	};

	if (options.includeSaves)
	{
		QString savesPath = FS::PathCombine(sourcePath, "saves");
		qDebug() << "BackupManager: checking saves path:" << savesPath;
		if (QDir(savesPath).exists())
		{
			collectFilesRecursive(savesPath);
		}
		else
		{
			qDebug() << "BackupManager: saves directory does not exist";
		}
	}
	if (options.includeConfig)
	{
		QString configPath = FS::PathCombine(sourcePath, "config");
		qDebug() << "BackupManager: checking config path:" << configPath;
		if (QDir(configPath).exists())
		{
			collectFilesRecursive(configPath);
		}
		else
		{
			qDebug() << "BackupManager: config directory does not exist";
		}
	}
	if (options.includeMods)
	{
		QString modsPath = FS::PathCombine(sourcePath, "mods");
		qDebug() << "BackupManager: checking mods path:" << modsPath;
		if (QDir(modsPath).exists())
		{
			collectFilesRecursive(modsPath);
		}
		else
		{
			qDebug() << "BackupManager: mods directory does not exist";
		}
	}
	if (options.includeResourcePacks)
	{
		QString rpPath = FS::PathCombine(sourcePath, "resourcepacks");
		qDebug() << "BackupManager: checking resourcepacks path:" << rpPath;
		if (QDir(rpPath).exists())
		{
			collectFilesRecursive(rpPath);
		}
		else
		{
			qDebug() << "BackupManager: resourcepacks directory does not exist";
		}
	}
	if (options.includeShaderPacks)
	{
		QString spPath = FS::PathCombine(sourcePath, "shaderpacks");
		qDebug() << "BackupManager: checking shaderpacks path:" << spPath;
		if (QDir(spPath).exists())
		{
			collectFilesRecursive(spPath);
		}
		else
		{
			qDebug() << "BackupManager: shaderpacks directory does not exist";
		}
	}
	if (options.includeScreenshots)
	{
		QString ssPath = FS::PathCombine(sourcePath, "screenshots");
		qDebug() << "BackupManager: checking screenshots path:" << ssPath;
		if (QDir(ssPath).exists())
		{
			collectFilesRecursive(ssPath);
		}
		else
		{
			qDebug() << "BackupManager: screenshots directory does not exist";
		}
	}
	if (options.includeOptions)
	{
		QFileInfo optionsFile(FS::PathCombine(sourcePath, "options.txt"));
		qDebug() << "BackupManager: checking options.txt:" << optionsFile.absoluteFilePath();
		if (optionsFile.exists())
		{
			qDebug() << "BackupManager: found options.txt";
			files.append(optionsFile);
		}
		else
		{
			qDebug() << "BackupManager: options.txt does not exist";
		}
		QFileInfo optionsOfFile(FS::PathCombine(sourcePath, "optionsof.txt"));
		qDebug() << "BackupManager: checking optionsof.txt:" << optionsOfFile.absoluteFilePath();
		if (optionsOfFile.exists())
		{
			qDebug() << "BackupManager: found optionsof.txt";
			files.append(optionsOfFile);
		}
		else
		{
			qDebug() << "BackupManager: optionsof.txt does not exist";
		}
	}

	// Include custom paths
	for (const QString& customPath : options.customPaths)
	{
		QString fullPath = FS::PathCombine(sourcePath, customPath);
		QFileInfo info(fullPath);

		if (info.isFile())
		{
			files.append(info);
		}
		else if (info.isDir())
		{
			collectFilesRecursive(fullPath);
		}
	}

	if (files.isEmpty())
	{
		qWarning() << "BackupManager: no files to backup!";
		// For new instances with no files, consider backup successful to allow launch
		return true;
	}

	qDebug() << "BackupManager: collected" << files.size() << "files";

	// Use MMCZip to compress - sourcePath is the base directory for relative paths
	bool result = MMCZip::compressDirFiles(backupPath, sourcePath, files);

	if (!result)
	{
		qWarning() << "BackupManager: MMCZip::compressDirFiles failed";
	}

	return result;
}

bool BackupManager::restoreBackup(InstancePtr instance, const InstanceBackup& backup, bool createBackupBeforeRestore)
{
	if (!instance || !backup.isValid())
	{
		qWarning() << "BackupManager: invalid instance or backup";
		return false;
	}

	// Get the game root (where .minecraft files are)
	auto minecraftInstance = std::dynamic_pointer_cast<MinecraftInstance>(instance);
	QString gameRoot	   = minecraftInstance ? minecraftInstance->gameRoot() : instance->instanceRoot();

	qDebug() << "BackupManager: restoring backup to game root:" << gameRoot;

	// Create safety backup before restore
	if (createBackupBeforeRestore)
	{
		qDebug() << "BackupManager: creating safety backup before restore";
		BackupOptions safetyOptions;
		safetyOptions.includeSaves		   = true;
		safetyOptions.includeConfig		   = true;
		safetyOptions.includeMods		   = true;
		safetyOptions.includeResourcePacks = true;
		safetyOptions.includeShaderPacks   = true;
		safetyOptions.includeScreenshots   = true;
		safetyOptions.includeOptions	   = true;
		createBackup(instance, "pre-restore_" + backup.name(), safetyOptions);
	}

	// Extract backup
	qDebug() << "BackupManager: extracting backup from" << backup.backupPath() << "to" << gameRoot;
	if (!extractBackup(backup.backupPath(), gameRoot))
	{
		qWarning() << "BackupManager: failed to extract backup";
		return false;
	}

	qDebug() << "BackupManager: backup restored successfully";
	emit backupRestored(instance->id(), backup.name());
	return true;
}

bool BackupManager::extractBackup(const QString& backupPath, const QString& targetPath)
{
	qDebug() << "BackupManager: extracting" << backupPath << "to" << targetPath;

	if (!QFile::exists(backupPath))
	{
		qWarning() << "BackupManager: backup file does not exist:" << backupPath;
		return false;
	}

	auto result = MMCZip::extractDir(backupPath, targetPath);

	if (!result.has_value())
	{
		qWarning() << "BackupManager: extraction failed";
		return false;
	}

	qDebug() << "BackupManager: extraction successful";
	return true;
}

QList<InstanceBackup> BackupManager::listBackups(InstancePtr instance) const
{
	if (!instance)
	{
		return {};
	}

	QString backupDir = getBackupDirectory(instance);
	return scanBackupDirectory(backupDir);
}

QList<InstanceBackup> BackupManager::scanBackupDirectory(const QString& backupDir) const
{
	QList<InstanceBackup> backups;
	QDir dir(backupDir);

	if (!dir.exists())
	{
		return backups;
	}

	QStringList zipFiles = dir.entryList(QStringList() << "*.zip", QDir::Files, QDir::Time);

	for (const QString& zipFile : zipFiles)
	{
		QString fullPath = dir.absoluteFilePath(zipFile);
		InstanceBackup backup(fullPath);

		if (backup.isValid())
		{
			backups.append(backup);
		}
	}

	return backups;
}

int BackupManager::deleteOldBackups(InstancePtr instance, int maxCount)
{
	if (!instance || maxCount < 1)
	{
		return 0;
	}

	QList<InstanceBackup> backups = listBackups(instance);

	if (backups.size() <= maxCount)
	{
		return 0;
	}

	// Sort by date (newest first)
	std::sort(backups.begin(),
			  backups.end(),
			  [](const InstanceBackup& a, const InstanceBackup& b) { return a.createdAt() > b.createdAt(); });

	int deletedCount = 0;
	for (int i = maxCount; i < backups.size(); i++)
	{
		if (deleteBackup(backups[i]))
		{
			deletedCount++;
		}
	}

	return deletedCount;
}

bool BackupManager::deleteBackup(const InstanceBackup& backup)
{
	if (!backup.isValid())
	{
		return false;
	}

	bool success = QFile::remove(backup.backupPath());
	QFile::remove(backup.backupPath() + ".json"); // Remove metadata too

	return success;
}

bool BackupManager::autoBackupBeforeLaunch(InstancePtr instance)
{
	BackupOptions options;
	options.includeSaves   = true;
	options.includeConfig  = true;
	options.includeOptions = true;
	options.includeMods	   = false; // Don't backup mods by default (too large)

	return createBackup(instance, "auto-backup-pre-launch", options);
}
