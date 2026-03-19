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

#include "ExtractNatives.hpp"
#include <launch/LaunchPipeline.hpp>
#include <minecraft/MinecraftInstance.h>

#include <quazip/quazip.h>
#include <quazip/quazipdir.h>
#include <quazip/quazipfile.h>
#include <quazip/quazipfileinfo.h>
#include <QDir>
#include "FileSystem.h"
#include "MMCZip.h"

#ifdef major
#undef major
#endif
#ifdef minor
#undef minor
#endif

static QString replaceSuffix(QString target, const QString& suffix, const QString& replacement)
{
	if (!target.endsWith(suffix))
	{
		return target;
	}
	target.resize(target.length() - suffix.length());
	return target + replacement;
}

static bool isWithinExtractionRoot(const QDir& root, const QString& path)
{
	const auto cleanRoot = QDir::cleanPath(root.absolutePath());
	const auto cleanPath = QDir::cleanPath(path);
	return cleanPath == cleanRoot || cleanPath.startsWith(cleanRoot + '/');
}

static bool symlinkEscapesExtractionRoot(QuaZip& zip, const QString& outputPath, const QDir& root)
{
	QuaZipFileInfo64 info;
	if (!zip.getCurrentFileInfo(&info) || !info.isSymbolicLink())
	{
		return false;
	}

	QuaZipFile linkFile(&zip);
	if (!linkFile.open(QIODevice::ReadOnly))
	{
		return true;
	}

	const auto linkTarget = QFile::decodeName(linkFile.readAll());
	linkFile.close();

	QString resolvedTarget;
	if (QDir::isAbsolutePath(linkTarget))
	{
		resolvedTarget = QDir::cleanPath(linkTarget);
	}
	else
	{
		const auto outputDir = QFileInfo(outputPath).dir();
		resolvedTarget	   = QDir::cleanPath(outputDir.absoluteFilePath(linkTarget));
	}

	return !isWithinExtractionRoot(root, resolvedTarget);
}

static bool unzipNatives(QString source, QString targetFolder, bool applyJnilibHack)
{
	QuaZip zip(source);
	if (!zip.open(QuaZip::mdUnzip))
	{
		return false;
	}
	QDir directory(targetFolder);
	if (!zip.goToFirstFile())
	{
		return false;
	}
	do
	{
		QString name   = zip.getCurrentFileName();
		auto lowercase = name.toLower();
		if (applyJnilibHack)
		{
			name = replaceSuffix(name, ".jnilib", ".dylib");
		}
		QString absFilePath = directory.absoluteFilePath(name);
		if (symlinkEscapesExtractionRoot(zip, absFilePath, directory))
		{
			return false;
		}
		if (!JlCompress::extractFile(&zip, "", absFilePath))
		{
			return false;
		}
	}
	while (zip.goToNextFile());
	zip.close();
	if (zip.getZipError() != 0)
	{
		return false;
	}
	return true;
}

void ExtractNatives::executeTask()
{
	auto instance  = m_flow->instance();
	auto toExtract = instance->getNativeJars();
	if (toExtract.isEmpty())
	{
		emitSucceeded();
		return;
	}
	auto settings = instance->settings();

	auto outputPath = instance->getNativePath();
	FS::ensureFolderPathExists(outputPath);
	auto javaVersion	= instance->getRuntimeVersion();
	bool jniHackEnabled = javaVersion.major() >= 8;
	for (const auto& source : toExtract)
	{
		if (!unzipNatives(source, outputPath, jniHackEnabled))
		{
			const char* reason = QT_TR_NOOP("Couldn't extract native jar '%1' to destination '%2'");
			emit logLine(QString(reason).arg(source, outputPath), MessageLevel::Fatal);
			emitFailed(tr(reason).arg(source, outputPath));
		}
	}
	emitSucceeded();
}

void ExtractNatives::finalize()
{
	auto instance	   = m_flow->instance();
	QString target_dir = FS::PathCombine(instance->instanceRoot(), "natives/");
	QDir dir(target_dir);
	dir.removeRecursively();
}
