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
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
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

#include "ExtractNatives.h"
#include <launch/LaunchTask.h>
#include <minecraft/MinecraftInstance.h>

#include <quazip/quazip.h>
#include <quazip/quazipdir.h>
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
	auto instance  = m_parent->instance();
	auto toExtract = instance->getNativeJars();
	if (toExtract.isEmpty())
	{
		emitSucceeded();
		return;
	}
	auto settings = instance->settings();

	auto outputPath = instance->getNativePath();
	FS::ensureFolderPathExists(outputPath);
	auto javaVersion	= instance->getJavaVersion();
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
	auto instance	   = m_parent->instance();
	QString target_dir = FS::PathCombine(instance->instanceRoot(), "natives/");
	QDir dir(target_dir);
	dir.removeRecursively();
}
