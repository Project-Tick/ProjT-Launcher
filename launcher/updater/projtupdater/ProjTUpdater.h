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
 *
 *
 *
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (C) 2023 Rachel Powers <508861+Ryex@users.noreply.github.com>
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
 *
 *
 * ======================================================================== */

#pragma once

#include <QtCore>

#include <QApplication>
#include <QDataStream>
#include <QDateTime>
#include <QDebug>
#include <QFlag>
#include <QIcon>
#include <QLocalSocket>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <memory>
#include <optional>

#include "QObjectPtr.h"
#include "net/Download.h"

#define PRISM_EXTERNAL_EXE
#include "FileSystem.h"

#include "ReleaseInfo.h"

class ProjTUpdaterApp : public QApplication
{
	// friends for the purpose of limiting access to deprecated stuff
	Q_OBJECT
  public:
	enum Status
	{
		Starting,
		Failed,
		Succeeded,
		Initialized,
		Aborted
	};
	ProjTUpdaterApp(int& argc, char** argv);
	virtual ~ProjTUpdaterApp();
	void loadReleaseFeed();
	void run();
	Status status() const
	{
		return m_status;
	}

  private:
	void fail(const QString& reason);
	void abort(const QString& reason);
	void showFatalErrorMessage(const QString& title, const QString& content);

	bool loadProjTVersionFromExe(const QString& exe_path);

	void downloadReleaseFeed();
	int parseReleaseFeed(const QByteArray& response);

	enum class UpdateKind
	{
		None,
		Update,
		Migration
	};

	struct UpdateCandidate
	{
		UpdateKind kind = UpdateKind::None;
		ReleaseInfo release;
	};

	UpdateCandidate findUpdateCandidate();

	ReleaseInfo getLatestRelease();
	ReleaseInfo selectRelease();
	QList<ReleaseInfo> newerReleases();
	QList<ReleaseInfo> nonDraftReleases();

	void printReleases();

	QList<ReleaseAsset> validReleaseArtifacts(const ReleaseInfo& release);
	ReleaseAsset selectAsset(const QList<ReleaseAsset>& assets);
	void performUpdate(const ReleaseInfo& release);
	void performInstall(QFileInfo file);
	void unpackAndInstall(QFileInfo file);
	void backupAppDir();
	std::optional<QDir> unpackArchive(QFileInfo file);

	QFileInfo downloadAsset(const ReleaseAsset& asset);
	bool callAppImageUpdate();

	void moveAndFinishUpdate(QDir target);

  public slots:
	void downloadError(QString reason);

  private:
	const QString& root()
	{
		return m_rootPath;
	}

	bool isPortable()
	{
		return m_isPortable;
	}

	void clearUpdateLog();
	void logUpdate(const QString& msg);

	QString m_rootPath;
	QString m_dataPath;
	bool m_isPortable = false;
	bool m_isAppimage = false;
	bool m_isFlatpak  = false;
	QString m_appimagePath;
	QString m_projtExecutable;
	QUrl m_releaseFeedUrl;
	Version m_userSelectedVersion;
	bool m_checkOnly;
	bool m_forceUpdate;
	bool m_printOnly;
	bool m_selectUI;
	bool m_allowDowngrade;
	bool m_allowPreRelease;

	QString m_updateLogPath;

	QString m_projtBinaryName;
	QString m_projtVersion;
	int m_projtVersionMajor = -1;
	int m_projtVersionMinor = -1;
	int m_projtVersionPatch = -1;
	QString m_prsimVersionChannel;
	QString m_projtGitCommit;

	ReleaseInfo m_install_release;

	Status m_status = Status::Starting;
	shared_qobject_ptr<QNetworkAccessManager> m_network;
	QString m_current_url;
	Task::Ptr m_current_task;
	QList<ReleaseInfo> m_releases;

  public:
	std::unique_ptr<QFile> logFile;
	bool logToConsole = false;

#if defined Q_OS_WIN32
	// used on Windows to attach the standard IO streams
	bool consoleAttached = false;
#endif
};
