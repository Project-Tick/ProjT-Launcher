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

#include <QDir>
#include <QFileSystemWatcher>
#include "Filter.h"

class RecursiveFileSystemWatcher : public QObject
{
	Q_OBJECT
  public:
	RecursiveFileSystemWatcher(QObject* parent);

	void setRootDir(const QDir& root);
	QDir rootDir() const
	{
		return m_root;
	}

	// WARNING: setting this to true may be bad for performance
	void setWatchFiles(bool watchFiles);
	bool watchFiles() const
	{
		return m_watchFiles;
	}

	void setMatcher(Filter matcher)
	{
		m_matcher = std::move(matcher);
	}

	QStringList files() const
	{
		return m_files;
	}

  signals:
	void filesChanged();
	void fileChanged(const QString& path);

  public slots:
	void enable();
	void disable();

  private:
	QDir m_root;
	bool m_watchFiles = false;
	bool m_isEnabled  = false;
	Filter m_matcher;

	QFileSystemWatcher* m_watcher;

	QStringList m_files;
	void setFiles(const QStringList& files);

	void addFilesToWatcherRecursive(const QDir& dir);
	QStringList scanRecursive(const QDir& dir);

  private slots:
	void fileChange(const QString& path);
	void directoryChange(const QString& path);
};
