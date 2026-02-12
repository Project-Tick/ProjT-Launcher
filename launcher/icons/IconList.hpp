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
#include <QDir>
#include <QFile>
#include <QMutex>
#include <QtGui/QIcon>
#include <memory>

#include "IconEntry.hpp"
#include "settings/Setting.h"

#include "QObjectPtr.h"

class QFileSystemWatcher;
class QMimeData;

namespace projt::icons
{
	class IconList : public QAbstractListModel
	{
		Q_OBJECT
	  public:
		explicit IconList(const QStringList& builtinPaths, const QString& path, QObject* parent = nullptr);
		~IconList() override = default;

		QIcon getIcon(const QString& key) const;
		int getIconIndex(const QString& key) const;
		QString getDirectory() const;

		QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
		int rowCount(const QModelIndex& parent = QModelIndex()) const override;

		QStringList mimeTypes() const override;
		Qt::DropActions supportedDropActions() const override;
		bool dropMimeData(const QMimeData* data,
						  Qt::DropAction action,
						  int row,
						  int column,
						  const QModelIndex& parent) override;
		Qt::ItemFlags flags(const QModelIndex& index) const override;

		bool addThemeIcon(const QString& key);
		bool addIcon(const QString& key, const QString& name, const QString& path, IconType type);
		void saveIcon(const QString& key, const QString& path, const char* format) const;
		bool deleteIcon(const QString& key);
		bool trashIcon(const QString& key);
		bool iconFileExists(const QString& key) const;
		QString iconDirectory(const QString& key) const;

		void installIcons(const QStringList& iconFiles);
		void installIcon(const QString& file, const QString& name);

		const IconEntry* icon(const QString& key) const;

		void startWatching();
		void stopWatching();

	  signals:
		void iconUpdated(QString key);

	  private:
		IconList(const IconList&)			 = delete;
		IconList& operator=(const IconList&) = delete;

		void reindex();
		void sortIconList();
		bool addPathRecursively(const QString& path);
		QStringList getIconFilePaths() const;

	  public slots:
		void directoryChanged(const QString& path);

	  protected slots:
		void fileChanged(const QString& path);
		void SettingChanged(const Setting& setting, const QVariant& value);

	  private:
		shared_qobject_ptr<QFileSystemWatcher> m_watcher;
		bool m_isWatching = false;
		QMap<QString, int> m_nameIndex;
		QList<IconEntry> m_icons;
		QDir m_dir;
	};
} // namespace projt::icons
